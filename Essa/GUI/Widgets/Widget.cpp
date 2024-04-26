#include <Essa/GUI/Widgets/Widget.hpp>

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/EML/AST.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Drawing/Outline.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/GUI/Overlays/Tooltip.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/Is.hpp>
#include <EssaUtil/Vector.hpp>
#include <cassert>
#include <iostream>
#include <typeinfo>

using namespace std::chrono_literals;

namespace GUI {

DBG_DEFINE_GLOBAL(GUI_DrawWidgetLayoutBounds);

Widget::~Widget() {
    if (m_tooltip) {
        m_tooltip->window().close();
    }
    if (m_window_root && m_window_root->focused_widget() == this)
        m_window_root->set_focused_widget(nullptr);
    if (EventLoop::has_current()) {
        EventLoop::current().remove_timer(m_tooltip_timer);
        EventLoop::current().remove_timer(m_double_click_timer);
    }
}

Util::Point2i Widget::local_mouse_position() const {
    auto mouse_pos_relative_to_host_window = llgl::mouse_position();
    return mouse_pos_relative_to_host_window - this->host_position().to_vector();
}

bool Widget::is_mouse_over(Util::Point2i mouse_pos) const {
    return absolute_rect().contains(mouse_pos);
}

Widget::EventHandlerResult Widget::do_handle_event(Event const& event) {
    auto transformed_event = event.relativized(absolute_position().to_vector());

    // Handle double click
    if (m_double_click_enabled) {
        if (auto const* mousepress = event.get<llgl::Event::MouseButtonPress>();
            mousepress && mousepress->button() == llgl::MouseButton::Left) {
            using namespace std::chrono_literals;
            if (!m_double_click_timer.expired()) {
                GUI::EventLoop::current().remove_timer(m_double_click_timer);
                return do_handle_event(GUI::Event::MouseDoubleClick(event.local_mouse_position()));
            }
            m_double_click_timer = GUI::EventLoop::current().set_timeout(400ms, []() {});
        }
    }

    // Check if widget is actually affected by the event, this
    // must be here so that event handler will actually run if
    // line below will change widget state (e.g hover)
    bool should_run_event_handler = is_affected_by_event(transformed_event);

    EventHandlerResult result = should_run_event_handler ? handle_event(transformed_event) : EventHandlerResult::NotAccepted;

    // Handle events common to all widgets
    auto result2 = transformed_event.visit(
        [&](Event::MouseEnter const&) -> EventHandlerResult { return EventHandlerResult::NotAccepted; },
        [&](Event::MouseLeave const&) -> EventHandlerResult {
            if (m_tooltip) {
                // FIXME: This makes tooltips not work at all because widget gets MouseLeave
                //        event immediately after tooltip is opened (For some reason)
                m_tooltip->window().close();
                m_tooltip = nullptr;
            }
            handle_event(GUI::Event::MouseLeave());
            return EventHandlerResult::NotAccepted;
        },
        [&](Event::MouseMove const& event) -> EventHandlerResult {
            auto mouse_position = event.local_position();
            bool new_hover = is_mouse_over(mouse_position + raw_position().to_vector());

            // Handle mouse enter/leave
            if (new_hover && !m_hover) {
                handle_event(GUI::Event::MouseEnter());
            }
            if (!new_hover && m_hover) {
                handle_event(GUI::Event::MouseLeave());
            }

            m_hover = new_hover;

            switch (m_tooltip_mode) {
            case TooltipMode::Hint: {
                if (m_hover && should_display_tooltip(mouse_position)) {
                    if (!m_tooltip) {
                        m_tooltip_position = mouse_position;
                        if (m_tooltip_timer.expired()) {
                            m_tooltip_timer = EventLoop::current().set_timeout(667ms, [this]() {
                                m_tooltip = &host_window().add_tooltip(
                                    this->host_position() + m_tooltip_position.to_vector(), Tooltip { create_tooltip(m_tooltip_position) }
                                );
                            });
                        }
                    }
                }
                else {
                    EventLoop::current().remove_timer(m_tooltip_timer);
                    if (m_tooltip) {
                        m_tooltip->window().close();
                        m_tooltip = nullptr;
                    }
                }
                break;
            }
            case TooltipMode::Realtime: {
                m_tooltip_position = mouse_position;
                if (m_hover && should_display_tooltip(mouse_position)) {
                    if (!m_tooltip) {
                        m_tooltip = &host_window().add_tooltip(
                            m_tooltip_position + host_position().to_vector(), Tooltip { create_tooltip(m_tooltip_position) }
                        );
                    }
                }
                else if (m_tooltip) {
                    m_tooltip->window().close();
                    m_tooltip = nullptr;
                }
                break;
            }
            }

            Util::Point2i tooltip_position { m_tooltip_position };

            if (m_tooltip) {
                // You will soon see why the API here is so twisted...
                auto text = m_tooltip->text();
                update_tooltip(mouse_position, text);
                m_tooltip->set_text(text);
                auto screen_tooltip_position = host_window().position() + absolute_position().to_vector() + tooltip_position.to_vector();
                static_cast<HostWindow&>(m_tooltip->window()).set_position(screen_tooltip_position + Util::Vector2i(32, 32));
            }

            return EventHandlerResult::NotAccepted;
        },
        [&](Event::MouseButtonPress const& event) -> EventHandlerResult {
            auto mouse_position = event.local_position();
            m_hover = is_mouse_over(mouse_position + raw_position().to_vector());
            if (m_hover) {
                m_hovered_on_click = true;
            }
            if (m_hover && accepts_focus()) {
                set_focused();
                return EventHandlerResult::Accepted;
            }
            return EventHandlerResult::NotAccepted;
        },
        [&](Event::MouseButtonRelease const& event) -> EventHandlerResult {
            auto mouse_position = event.local_position();
            m_hover = is_mouse_over(mouse_position + raw_position().to_vector());
            m_hovered_on_click = false;
            return EventHandlerResult::NotAccepted;
        },
        [&](auto const&) -> EventHandlerResult { return EventHandlerResult::NotAccepted; }
    );

    return result2 == EventHandlerResult::Accepted || result == EventHandlerResult::Accepted ? EventHandlerResult::Accepted
                                                                                             : EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult Widget::handle_event(Event const& event) {
    return event.visit(
        [&](Event::KeyPress const& event) -> EventHandlerResult { return on_key_press(event); },
        [&](Event::KeyRelease const& event) -> EventHandlerResult { return on_key_release(event); },
        [&](Event::MouseMove const& event) -> EventHandlerResult { return on_mouse_move(event); },
        [&](Event::MouseButtonPress const& event) -> EventHandlerResult { return on_mouse_button_press(event); },
        [&](Event::MouseButtonRelease const& event) -> EventHandlerResult { return on_mouse_button_release(event); },
        [&](Event::MouseScroll const& event) -> EventHandlerResult { return on_mouse_scroll(event); },
        [&](Event::TextInput const& event) -> EventHandlerResult { return on_text_input(event); },
        [&](Event::MouseDoubleClick const& event) -> EventHandlerResult { return on_mouse_double_click(event); },
        [&](Event::MouseEnter const& event) -> EventHandlerResult { return on_mouse_enter(event); },
        [&](Event::MouseLeave const& event) -> EventHandlerResult { return on_mouse_leave(event); }
    );
}

void Widget::do_update() {
    Widget::update();
    update();
}

void Widget::set_focused() {
    assert(accepts_focus());
    m_window_root->set_focused_widget(this);
}

bool Widget::is_focused() const {
    return m_window_root->focused_widget() == this;
}

void Widget::focus_first_child_or_self() {
    set_focused();
}

bool Widget::are_all_parents_enabled() const {
    return is_enabled() && (m_parent ? m_parent->is_enabled() : true);
}

bool Widget::is_affected_by_event(Event const& event) const {
    if (!are_all_parents_enabled()) {
        return false;
    }

    switch (event.target_type()) {
    case llgl::EventTargetType::KeyboardFocused:
        return is_focused();
    case llgl::EventTargetType::MouseFocused:
        return local_rect().contains(event.local_mouse_position()) || m_hovered_on_click;
    case llgl::EventTargetType::Specific:
        return false;
    case llgl::EventTargetType::Window:
        return true;
    }
    ESSA_UNREACHABLE;
}

void Widget::do_draw(Gfx::Painter& painter) const {
    Gfx::ClipViewScope scope(painter, parent_relative_rect(), Gfx::ClipViewScope::Mode::Intersect);

    painter.draw(Gfx::Drawing::Rectangle {
        local_rect().cast<float>(),
        Gfx::Drawing::Fill {}.set_color(m_background_color),
        Gfx::Drawing::Outline::none(),
    });

    this->draw(painter);

    if (DBG_ENABLED(GUI_DrawWidgetLayoutBounds)) {
        using namespace Gfx::Drawing;
        painter.draw(Rectangle(local_rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Magenta, -1)));
    }
}

Util::Point2i Widget::screen_position() const {
    return host_window().position() + host_position().to_vector();
}

Util::Point2i Widget::host_position() const {
    return m_window_root->window().host_position() + absolute_position().to_vector();
}

Util::Point2i Widget::absolute_position() const {
    return m_position + (m_parent ? m_parent->absolute_position().to_vector() : Util::Vector2i());
}

Util::Point2i Widget::parent_relative_position() const {
    return m_position;
}

void Widget::set_parent_relative_position(Util::Point2i p) {
    m_position = p;
}

void Widget::set_raw_position(Util::Point2i position) {
    m_position = position - (m_parent ? m_parent->raw_position().to_vector() : Util::Vector2i());
}

void Widget::do_relayout() {
    copy_initial_sizes();
    if (this->m_visible)
        this->relayout();
    // std::cout << "do_relayout "  << this << ":" << typeid(*this).name() <<
    // m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y <<
    // std::endl;
}

void Widget::set_needs_relayout() {
    if (m_window_root) {
        m_window_root->set_needs_relayout();
    }
}

void Widget::copy_initial_sizes() {
    auto initial_size = this->initial_size();
    if (m_input_size.x == Util::Length::Initial)
        m_input_size.x = initial_size.x;
    if (m_input_size.y == Util::Length::Initial)
        m_input_size.y = initial_size.y;
}

Theme const& Widget::theme() const {
    return Application::the().theme();
}

Gfx::ResourceManager const& Widget::resource_manager() const {
    return Application::the().resource_manager();
}

HostWindow& Widget::host_window() const {
    auto& window = m_window_root->window();
    return window.host_window();
}

void Widget::set_parent(Widget& parent) {
    set_window_root(parent.window_root());
    m_parent = &parent;
}

void Widget::dump(std::ostream& out, unsigned depth) {
    auto it = std::ostream_iterator<char>(out);

    for (unsigned i = 0; i < depth; i++) {
        fmt::format_to(it, "-   ");
    }
    fmt::format_to(it, "({}) ", (m_visible ? "-" : "+"));
    fmt::format_to(it, "{}", typeid(*this).name());
    if (!m_id.empty()) {
        fmt::format_to(it, " #{}", m_id);
    }
    fmt::format_to(it, ": pos=({}, {})={}", fmt::streamed(m_input_position.x), fmt::streamed(m_input_position.y), m_position);
    fmt::format_to(it, ", size=({}, {})={}", fmt::streamed(m_input_size.x), fmt::streamed(m_input_size.y), m_raw_size);
    fmt::format_to(it, "\n");
}

EML::EMLErrorOr<void> Widget::load_from_eml_object(EML::Object const& object, EML::Loader&) {
    m_id = object.id;
    m_tooltip_text = TRY(object.get_property("tooltip_text", EML::Value("")).to_string());
    m_vertical_alignment = TRY(object.get_enum<Alignment>("vertical_alignment", alignment_from_string, Alignment::Start));
    m_horizontal_alignment = TRY(object.get_enum<Alignment>("horizontal_alignment", alignment_from_string, Alignment::Start));
    m_input_size.x = TRY(object.get_property("width", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_input_size.y = TRY(object.get_property("height", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_input_position.x = TRY(object.get_property("left", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_input_position.y = TRY(object.get_property("top", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_background_color = TRY(object.get_property("background_color", EML::Value(Util::Color { 0x000000 })).to_color());
    m_enabled = TRY(object.get_property("enabled", EML::Value(true)).to_bool());
    m_visible = TRY(object.get_property("visible", EML::Value(true)).to_bool());
    return {};
}

std::vector<DevToolsObject const*> Widget::dev_tools_children() const {
    return {};
}

Util::UString Widget::dev_tools_name() const {
    return Util::UString(id());
}

EML_REGISTER_CLASS(Widget);

}
