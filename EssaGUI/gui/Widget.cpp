#include "Widget.hpp"

#include "Application.hpp"
#include "Container.hpp"
#include "Tooltip.hpp"
#include <EssaGUI/gfx/ClipViewScope.hpp>

#include <cassert>
#include <iostream>
#include <typeinfo>

namespace GUI {

Widget::Widget(Container& parent)
    : m_parent(&parent)
    , m_widget_tree_root(parent.m_widget_tree_root) {
}

Widget::~Widget() {
    if (m_widget_tree_root.focused_widget() == this)
        m_widget_tree_root.set_focused_widget(nullptr);
}

bool Widget::is_mouse_over(Util::Vector2i mouse_pos) const {
    return Util::Rectf { m_pos, m_size }.contains(mouse_pos);
}

void Widget::update() {
    if (!m_tooltip_text.is_empty()) {
        // std::cout << this << ": " << m_tooltip_counter << std::endl;
        if (m_tooltip_counter > 0)
            m_tooltip_counter--;
        if (m_hover) {
            if (m_tooltip_counter == 0 && !m_tooltip) {
                // TODO: Use mouse position;
                m_tooltip = &Application::the().add_tooltip(Tooltip { m_tooltip_text, this, position() + m_widget_tree_root.position() });
                // std::cout << m_tooltip << std::endl;
                m_tooltip_counter = -1;
            }
        }
        else if (m_tooltip_counter == 0) {
            // std::cout << "TEST " << this << " " << m_tooltip << std::endl;
            if (m_tooltip) {
                m_tooltip->close();
                m_tooltip = nullptr;
            }
            m_tooltip_counter = -1;
        }
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
    m_widget_tree_root.set_focused_widget(this);
}

bool Widget::is_focused() const {
    return m_widget_tree_root.focused_widget() == this;
}

void Widget::focus_first_child_or_self() {
    set_focused();
}

bool Widget::are_all_parents_enabled() const {
    return is_enabled() && (m_parent ? m_parent->is_enabled() : true);
}

void Widget::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::MouseMove) {
        Util::Vector2i mouse_pos = event.event().mouse_move.position;
        bool previous_hover = m_hover;
        m_hover = is_mouse_over(mouse_pos);
        if (previous_hover != m_hover) {
            if (m_tooltip)
                m_tooltip_counter = 30;
            else
                m_tooltip_counter = 90;
            event.set_seen();
        }
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
    return { position() + m_widget_tree_root.position(), size() };
}

void Widget::do_relayout() {
    if (this->m_visible)
        this->relayout();
    // std::cout << "do_relayout "  << this << ":" << typeid(*this).name() << m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y << std::endl;
}

void Widget::set_needs_relayout() {
    m_widget_tree_root.set_needs_relayout();
}

GUI::Window& Widget::window() const {
    return m_widget_tree_root.window();
}

Theme const& Widget::theme() const {
    return Application::the().theme();
}

Gfx::ResourceManager const& Widget::resource_manager() const {
    return Application::the().resource_manager();
}

void Widget::dump(unsigned depth) {
    for (unsigned i = 0; i < depth; i++)
        std::cout << "-   ";
    std::cout << (m_visible ? "(-) " : "(+) ");
    std::cout << typeid(*this).name() << " @" << this;
    if (!m_id.empty())
        std::cout << " #" << m_id;
    std::cout << ": pos=" << m_pos.x() << "," << m_pos.y() << " size=" << m_size.x() << "," << m_size.y();
    std::cout << std::endl;
}

}
