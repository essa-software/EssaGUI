#include "Tooltip.hpp"

#include "Application.hpp"

namespace GUI {

void TooltipOverlay::draw() {
    sf::Text text(m_tooltip.text, Application::the().font, 15);
    text.setFillColor(sf::Color::Black);
    text.setPosition(position());

    auto bounds = text.getGlobalBounds();

    sf::RectangleShape bg { { bounds.width + 10, bounds.height + 10 } };
    auto x_pos = std::min(window().getSize().x - bg.getSize().x, bounds.left - 5);
    bg.setPosition(x_pos, bounds.top - 5);
    text.setPosition(x_pos + 5, text.getPosition().y);
    window().draw(bg);

    window().draw(text);
}

}
