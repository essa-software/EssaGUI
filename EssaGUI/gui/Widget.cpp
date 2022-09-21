#include "Widget.hpp"

#include "Application.hpp"
#include "Container.hpp"
#include "Tooltip.hpp"
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/Is.hpp>
#include <EssaUtil/Vector.hpp>
#include <cassert>
#include <iostream>
#include <typeinfo>

namespace GUI {

Widget::~Widget() {
    if (m_widget_tree_root && m_widget_tree_root->focused_widget() == this)
        m_widget_tree_root->set_focused_widget(nullptr);
}

bool Widget::is_mouse_over(Util::Vector2i mouse_pos) const {
    return Util::Rectf { m_raw_position, m_raw_size }.contains(mouse_pos);
}

void Widget::update() {
    if (!m_tooltip_text.is_empty()) {
        if (m_tooltip_counter > 0)
            m_tooltip_counter--;
        if (m_tooltip_counter == 0) {
            // std::cout << "TEST " << this << " " << m_tooltip << std::endl;
            if (m_tooltip) {
                m_tooltip->close();
                m_tooltip = nullptr;
            }
            else {
                m_tooltip = &host_window().add_tooltip(Tooltip { m_tooltip_text, {} });
            }
            m_tooltip_counter = -1;
        }
    }

    if (m_tooltip) {
        m_tooltip->set_position(m_widget_tree_root->position() + Util::Vector2f { m_tooltip_position } + Util::Vector2f { 32, 32 });
    }
}

void Widget::do_handle_event(Event& event) {
    Widget::handle_event(event);
    handle_event(event);
}

void Widget::do_update() {
    Widget::update();
    update();
}

void Widget::set_focused() {
    assert(accepts_focus());
    m_widget_tree_root->set_focused_widget(this);
}

bool Widget::is_focused() const {
    return m_widget_tree_root->focused_widget() == this;
}

void Widget::focus_first_child_or_self() {
    set_focused();
}

bool Widget::are_all_parents_enabled() const {
    return is_enabled() && (m_parent ? m_parent->is_enabled() : true);
}

void Widget::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::MouseMove) {
        auto mouse_position = event.mouse_position();
        m_hover = is_mouse_over(mouse_position);
        switch (m_tooltip_mode) {
        case TooltipMode::Hint: {
            if (m_hover && !m_tooltip) {
                m_tooltip_counter = 40;
                m_tooltip_position = mouse_position;
            }
            if (!m_hover) {
                if (m_tooltip)
                    m_tooltip_counter = 0;
                else
                    m_tooltip_counter = -1;
            }
            break;
        }
        case TooltipMode::Realtime: {
            if (m_hover) {
                if (!m_tooltip)
                    m_tooltip_counter = 0;
                else
                    m_tooltip_position = mouse_position;
            }
            else if (m_tooltip) {
                m_tooltip_counter = 0;
            }
            break;
        }
        }
        event.set_seen();
    }
    else if (event.type() == llgl::Event::Type::MouseButtonPress) {
        Util::Vector2i mouse_pos = event.event().mouse_button.position;
        m_hover = is_mouse_over(mouse_pos);
        if (m_hover && accepts_focus()) {
            set_focused();
            event.set_handled();
        }
    }
    else if (event.type() == llgl::Event::Type::MouseButtonRelease) {
        Util::Vector2i mouse_pos = event.event().mouse_button.position;
        m_hover = is_mouse_over(mouse_pos);
    }
    else if (event.type() == llgl::Event::Type::Resize)
        set_needs_relayout();
}

void Widget::do_draw(GUI::Window& window) const {
    // std::cout << "do_draw "  << this << ":" << typeid(*this).name() << m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y << std::endl;
    Gfx::ClipViewScope scope(window, rect(), Gfx::ClipViewScope::Mode::Intersect);

    RectangleDrawOptions background;
    background.fill_color = m_background_color;
    window.draw_rectangle(local_rect(), background);

    this->draw(window);
}

Util::Rectf Widget::rect() const {
    return { raw_position() + m_widget_tree_root->position(), raw_size() };
}

void Widget::do_relayout() {
    if (this->m_visible)
        this->relayout();
    // std::cout << "do_relayout "  << this << ":" << typeid(*this).name() << m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y << std::endl;
}

void Widget::set_needs_relayout() {
    m_widget_tree_root->set_needs_relayout();
}

Theme const& Widget::theme() const {
    return Application::the().theme();
}

Gfx::ResourceManager const& Widget::resource_manager() const {
    return Application::the().resource_manager();
}

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
    for (unsigned i = 0; i < depth; i++)
        std::cout << "-   ";
    std::cout << (m_visible ? "(-) " : "(+) ");
    std::cout << typeid(*this).name() << " @" << this;
    if (!m_id.empty())
        std::cout << " #" << m_id;
    std::cout << ": pos=" << m_raw_position.x() << "," << m_raw_position.y() << " size=" << m_raw_position.x() << "," << m_raw_position.y();
    std::cout << std::endl;
}

EML::EMLErrorOr<void> Widget::load_from_eml_object(EML::Object const& object, EML::Loader&) {
    m_id = object.id;
    m_tooltip_text = TRY(object.get_property("tooltip_text", Util::UString {}).to_string());
    // TODO: Tooltip mode
    m_input_size.x = TRY(object.get_property("width", Length { Length::Initial }).to_length());
    m_input_size.y = TRY(object.get_property("height", Length { Length::Initial }).to_length());
    m_expected_pos.x = TRY(object.get_property("left", Length { Length::Initial }).to_length());
    m_expected_pos.y = TRY(object.get_property("top", Length { Length::Initial }).to_length());
    m_background_color = TRY(object.get_property("background_color", Util::Color { 0x000000 }).to_color());
    m_enabled = TRY(object.get_property("enabled", true).to_bool());
    m_visible = TRY(object.get_property("visible", true).to_bool());
    return {};
}
}
