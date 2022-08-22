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

Theme::BgFgTextColors Button::colors_for_state() const {
    auto colors = m_button_colors_override.value_or(default_button_colors()).value(*this);
    return colors;
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
