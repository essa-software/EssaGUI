#include "Frame.hpp"

#include <EssaGUI/gfx/RoundedEdgeRectangleShape.hpp>

#include <iostream>

namespace GUI {

void Frame::draw(sf::RenderWindow& window) const {
    RoundedEdgeRectangleShape rers(size(), BorderRadius);
    rers.setFillColor(sf::Color(50, 50, 50, 150));
    window.draw(rers);
}

}
