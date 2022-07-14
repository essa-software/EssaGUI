#include "Button.hpp"

#include <iostream>

namespace GUI {

void Button::handle_event(Event& event) {
    if (event.event().type == llgl::Event::Type::MouseButtonPress && is_hover())
        m_pressed_on_button = true;
    else if (event.event().type == llgl::Event::Type::MouseButtonRelease && is_hover() && m_pressed_on_button) {
        click();
        m_pressed_on_button = false;
    }
}

Util::Color Button::text_color_for_state() const {
    auto colors = m_button_colors_override.value_or(default_button_colors());
    return color_for_state(m_toggleable
            ? (m_active ? colors.active.text : colors.inactive.text)
            : colors.untoggleable.text);
}

Util::Color Button::bg_color_for_state() const {
    auto colors = m_button_colors_override.value_or(default_button_colors());
    return color_for_state(m_toggleable
            ? (m_active ? colors.active.background : colors.inactive.background)
            : colors.untoggleable.background);
}

Util::Color Button::color_for_state(Util::Color color) const {
    if (is_hover())
        color = color + Util::Color { 20, 20, 20, 0 };
    return color;
}

void Button::click() {
    if (m_toggleable) {
        m_active = !m_active;
        if (on_change)
            on_change(m_active);
    }
    if (on_click)
        on_click();
}

}
