#include <Essa/GUI/Widgets/Widget.hpp>

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Drawing/Outline.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
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
    if (m_widget_tree_root && m_widget_tree_root->focused_widget() == this)
        m_widget_tree_root->set_focused_widget(nullptr);
    if (EventLoop::has_current()) {
        EventLoop::current().remove_timer(m_tooltip_timer);
    }
}

bool Widget::is_mouse_over(Util::Cs::Point2i mouse_pos) const { return Util::Recti(raw_position(), m_raw_size).contains(mouse_pos); }

void Widget::update() {
    Util::Cs::Point2i tooltip_position { m_tooltip_position };
    auto widget_relative_mouse_position = Util::Cs::Point2i::from_deprecated_vector(llgl::mouse_position());
    widget_relative_mouse_position -= widget_tree_root().position().to_vector() + raw_position().to_vector();

    if (m_tooltip) {
        // You will soon see why the API here is so twisted...
        auto text = m_tooltip->text();
        update_tooltip(widget_relative_mouse_position, text);
        m_tooltip->set_text(text);
        m_tooltip->set_position(
            (m_widget_tree_root->position() + raw_position().to_vector() + tooltip_position.to_vector() + Util::Cs::Vector2i(32, 32))
        );
    }
}

Widget::EventHandlerResult Widget::do_handle_event(Event const& event) {
    auto transformed_event = event.relativized(raw_position().to_vector());

    // Check if widget is actually affected by the event, this
    // must be here so that event handler will actually run if
    // line below will change widget state (e.g hover)
    bool should_run_event_handler = is_affected_by_event(transformed_event);
    EventHandlerResult result = should_run_event_handler ? handle_event(transformed_event) : EventHandlerResult::NotAccepted;

    // Handle events common to all widgets
    auto result2 = transformed_event.visit(
        [&](Event::MouseMove const& event) -> EventHandlerResult {
            auto mouse_position = event.local_position();
            m_hover = is_mouse_over(mouse_position + raw_position().to_vector());
            switch (m_tooltip_mode) {
            case TooltipMode::Hint: {
                auto widget_relative_mouse_position = Util::Cs::Point2i::from_deprecated_vector(llgl::mouse_position());
                widget_relative_mouse_position -= host_position().to_vector();
                if (m_hover && should_display_tooltip(widget_relative_mouse_position)) {
                    if (!m_tooltip) {
                        m_tooltip_position = widget_relative_mouse_position;
                        if (m_tooltip_timer.expired()) {
                            m_tooltip_timer = EventLoop::current().set_timeout(667ms, [this]() {
                                fmt::print("TIMER\n");
                                m_tooltip = &host_window().add_tooltip(Tooltip { create_tooltip(m_tooltip_position), {} });
                            });
                        }
                    }
                }
                else {
                    EventLoop::current().remove_timer(m_tooltip_timer);
                    if (m_tooltip) {
                        m_tooltip->close();
                        m_tooltip = nullptr;
                    }
                }
                break;
            }
            case TooltipMode::Realtime: {
                m_tooltip_position = mouse_position;
                if (m_hover && should_display_tooltip(mouse_position)) {
                    if (!m_tooltip) {
                        m_tooltip = &host_window().add_tooltip(Tooltip { create_tooltip(m_tooltip_position), {} });
                    }
                }
                else if (m_tooltip) {
                    m_tooltip->close();
                    m_tooltip = nullptr;
                }
                break;
            }
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
        [&](Event::WindowResize const&) -> EventHandlerResult {
            set_needs_relayout();
            return EventHandlerResult::NotAccepted;
        },
        [&](auto const&) -> EventHandlerResult { return EventHandlerResult::NotAccepted; }
    );

    return result2 == EventHandlerResult::Accepted || result == EventHandlerResult::Accepted ? EventHandlerResult::Accepted
                                                                                             : EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult Widget::handle_event(Event const& event) {
    return event.visit(
        [&](Event::WindowResize const& event) -> EventHandlerResult { return on_window_resize(event); },
        [&](Event::KeyPress const& event) -> EventHandlerResult { return on_key_press(event); },
        [&](Event::KeyRelease const& event) -> EventHandlerResult { return on_key_release(event); },
        [&](Event::MouseMove const& event) -> EventHandlerResult { return on_mouse_move(event); },
        [&](Event::MouseButtonPress const& event) -> EventHandlerResult { return on_mouse_button_press(event); },
        [&](Event::MouseButtonRelease const& event) -> EventHandlerResult { return on_mouse_button_release(event); },
        [&](Event::MouseScroll const& event) -> EventHandlerResult { return on_mouse_scroll(event); },
        [&](Event::TextInput const& event) -> EventHandlerResult { return on_text_input(event); }
    );
}

void Widget::do_update() {
    Widget::update();
    update();
}

void Widget::set_focused() {
    assert(accepts_focus());
    m_widget_tree_root->set_focused_widget(this);
}

bool Widget::is_focused() const { return m_widget_tree_root->focused_widget() == this; }

void Widget::focus_first_child_or_self() { set_focused(); }

bool Widget::are_all_parents_enabled() const { return is_enabled() && (m_parent ? m_parent->is_enabled() : true); }

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
    case llgl::EventTargetType::Global:
        return true;
    }
    ESSA_UNREACHABLE;
}

void Widget::do_draw(Gfx::Painter& painter) const {
    Gfx::ClipViewScope scope(painter, parent_relative_rect(), Gfx::ClipViewScope::Mode::Intersect);

    Gfx::RectangleDrawOptions background;
    background.fill_color = m_background_color;
    painter.deprecated_draw_rectangle(local_rect().cast<float>(), background);

    this->draw(painter);

    if (DBG_ENABLED(GUI_DrawWidgetLayoutBounds)) {
        using namespace Gfx::Drawing;
        painter.draw(Rectangle(local_rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Magenta, -1)));
    }
}

Util::Cs::Point2i Widget::raw_position() const {
    return m_position + (m_parent ? m_parent->raw_position().to_vector() : Util::Cs::Vector2i());
}

void Widget::set_raw_position(Util::Cs::Point2i position) {
    m_position = position - (m_parent ? m_parent->raw_position().to_vector() : Util::Cs::Vector2i());
}

Util::Recti Widget::host_rect() const {
    return {
        raw_position() + m_widget_tree_root->position().to_vector(),
        raw_size(),
    };
}

Util::Recti Widget::parent_relative_rect() const { return { m_position, m_raw_size }; }

Util::Recti Widget::absolute_rect() const { return { raw_position(), raw_size() }; }

void Widget::do_relayout() {
    if (this->m_visible)
        this->relayout();
    // std::cout << "do_relayout "  << this << ":" << typeid(*this).name() <<
    // m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y <<
    // std::endl;
}

void Widget::set_needs_relayout() { m_widget_tree_root->set_needs_relayout(); }

Theme const& Widget::theme() const { return Application::the().theme(); }

Gfx::ResourceManager const& Widget::resource_manager() const { return Application::the().resource_manager(); }

HostWindow& Widget::host_window() const {
    if (Util::is<HostWindow>(*m_widget_tree_root))
        return static_cast<HostWindow&>(*m_widget_tree_root);
    if (Util::is<Overlay>(*m_widget_tree_root))
        return static_cast<Overlay&>(*m_widget_tree_root).host_window();
    // WTR can currently be only HostWindow or Overlay
    ESSA_UNREACHABLE;
}

void Widget::set_parent(Container& parent) {
    set_widget_tree_root(parent.widget_tree_root());
    m_parent = &parent;
}

void Widget::dump(unsigned depth) {
    for (unsigned i = 0; i < depth; i++) {
        fmt::print("-   ");
    }
    fmt::print("({}) ", (m_visible ? "-" : "+"));
    fmt::print("{} @{}", typeid(*this).name(), fmt::ptr(this));
    if (!m_id.empty()) {
        fmt::print(" #{}", m_id);
    }
    fmt::print(": pos=({}, {})={}", fmt::streamed(m_expected_pos.x), fmt::streamed(m_expected_pos.y), m_position);
    fmt::print(", size=({}, {})={}", fmt::streamed(m_input_size.x), fmt::streamed(m_input_size.y), m_raw_size);
    fmt::print("\n");
}

EML::EMLErrorOr<void> Widget::load_from_eml_object(EML::Object const& object, EML::Loader&) {
    m_id = object.id;
    m_tooltip_text = TRY(object.get_property("tooltip_text", EML::Value("")).to_string());
    m_vertical_alignment = TRY(object.get_enum<Alignment>("vertical_alignment", alignment_from_string, Alignment::Start));
    m_horizontal_alignment = TRY(object.get_enum<Alignment>("horizontal_alignment", alignment_from_string, Alignment::Start));
    m_input_size.x = TRY(object.get_property("width", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_input_size.y = TRY(object.get_property("height", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_expected_pos.x = TRY(object.get_property("left", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_expected_pos.y = TRY(object.get_property("top", EML::Value(Util::Length { Util::Length::Initial })).to_length());
    m_background_color = TRY(object.get_property("background_color", EML::Value(Util::Color { 0x000000 })).to_color());
    m_enabled = TRY(object.get_property("enabled", EML::Value(true)).to_bool());
    m_visible = TRY(object.get_property("visible", EML::Value(true)).to_bool());
    return {};
}

}
