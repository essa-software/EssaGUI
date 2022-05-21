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
    return is_active() ? m_active_text_color : m_text_color;
}

sf::Color Button::bg_color_for_state() const {
    sf::Color base_color = is_active() ? m_active_bg_color : m_bg_color;
    if (is_hover())
        base_color += sf::Color { 20, 20, 20, 0 };
    return base_color;
}

sf::Color Button::fg_color_for_state() const {
    sf::Color base_color = is_active() ? m_active_fg_color : m_fg_color;
    if (is_hover())
        base_color += sf::Color { 20, 20, 20, 0 };
    return base_color;
}

void Button::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Button::set_active_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
    m_active_bg_color = bg_color;
    m_active_fg_color = fg_color;
    m_active_text_color = text_color;
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
