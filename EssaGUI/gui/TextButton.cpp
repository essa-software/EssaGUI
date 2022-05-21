#include "TextButton.hpp"

#include "Application.hpp"
#include "Button.hpp"

namespace GUI {

TextButton::TextButton(Container& c)
    : Button(c) {
    set_display_attributes(sf::Color(200, 80, 80), sf::Color::Blue, sf::Color::White);
    set_active_display_attributes(sf::Color(80, 200, 80), sf::Color::Blue, sf::Color::White);
}

void TextButton::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rect(size());
    rect.setFillColor(bg_color_for_state());
    rect.setOutlineColor(fg_color_for_state());
    rect.setOutlineThickness(3);
    window.draw(rect);

    sf::Text text(m_content, Application::the().font, 15);
    text.setFillColor(text_color_for_state());
    align_text(m_alignment, size(), text);

    if (is_active())
        text.setString(m_active_content);
    window.draw(text);
}

}
