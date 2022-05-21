#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

namespace GUI {

enum class Align {
    TopLeft,
    Top,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    Bottom,
    BottomRight,
};

inline void align_text(Align alignment, sf::Vector2f size, sf::Text& text) {
    auto bounds = text.getLocalBounds();

    // The -5 is needed because SFML doesn't take descenders into account
    switch (alignment) {
    case Align::TopLeft:
        text.setPosition(sf::Vector2f(2, -3));
        break;
    case Align::Top:
        text.setPosition(sf::Vector2f(std::round(size.x / 2 - bounds.width / 2), -5));
        break;
    case Align::TopRight:
        text.setPosition(sf::Vector2f(std::round(size.x - bounds.width - 2), -5));
        break;
    case Align::CenterLeft:
        text.setPosition(sf::Vector2f(2, std::round(size.y / 2 - bounds.height / 2 - 5)));
        break;
    case Align::Center:
        text.setPosition(sf::Vector2f(std::round(size.x / 2 - bounds.width / 2), std::round(size.y / 2 - bounds.height / 2 - 5)));
        break;
    case Align::CenterRight:
        text.setPosition(sf::Vector2f(std::round(size.x - bounds.width - 2), std::round(size.y / 2 - bounds.height / 2 - 5)));
        break;
    case Align::BottomLeft:
        text.setPosition(sf::Vector2f(2, std::round(2 + size.y - bounds.height - 5)));
        break;
    case Align::Bottom:
        text.setPosition(sf::Vector2f(std::round(size.x / 2 - bounds.width / 2), std::round(size.y - bounds.height - 5)));
        break;
    case Align::BottomRight:
        text.setPosition(sf::Vector2f(std::round(size.x - bounds.width - 2), std::round(size.y - bounds.height - 5)));
        break;
    }
}

}
