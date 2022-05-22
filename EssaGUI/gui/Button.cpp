#include "Button.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

namespace GUI {

void Button::handle_event(Event& event) {
    if (event.event().type == sf::Event::MouseButtonPressed && is_hover())
        m_pressed_on_button = true;
    else if (event.event().type == sf::Event::MouseButtonReleased && is_hover() && m_pressed_on_button) {
        click();
        m_pressed_on_button = false;
    }
}

sf::Color Button::text_color_for_state() const {
    auto colors = m_button_colors_override.value_or(default_button_colors());
    return color_for_state(m_toggleable
            ? (m_active ? colors.active.text : colors.inactive.text)
            : colors.untoggleable.text);
}

sf::Color Button::bg_color_for_state() const {
    auto colors = m_button_colors_override.value_or(default_button_colors());
    return color_for_state(m_toggleable
            ? (m_active ? colors.active.background : colors.inactive.background)
            : colors.untoggleable.background);
}

sf::Color Button::color_for_state(sf::Color color) const {
    if (is_hover())
        color += sf::Color { 20, 20, 20, 0 };
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
