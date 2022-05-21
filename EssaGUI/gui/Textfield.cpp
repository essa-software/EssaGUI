#include "Textfield.hpp"
#include "Application.hpp"
#include "TextAlign.hpp"
#include <SFML/System/Vector2.hpp>

namespace GUI {

void Textfield::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Textfield::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rect(size());
    rect.setFillColor(m_bg_color);
    /*rect.setOutlineColor(m_fg_color);
    rect.setOutlineThickness(3);*/

    window.draw(rect);

    sf::Text text(m_content, Application::the().font, m_font_size);
    text.setFillColor(m_text_color);

    align_text(m_alignment, size(), text);

    window.draw(text);
}

}
