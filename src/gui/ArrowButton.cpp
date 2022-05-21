#include "ArrowButton.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

namespace GUI {

void ArrowButton::draw(sf::RenderWindow& window) const {
    sf::RectangleShape background(size());
    background.setFillColor(bg_color_for_state());
    window.draw(background);

    const sf::Vector2f midpoint = size() / 2.f;

    // std::cout << Vector3(midpoint) << "\n";

    const sf::Vector2f points[8] = {
        sf::Vector2f(std::round(midpoint.x - m_arrow_size / 2), std::round(midpoint.y - m_arrow_size / 2)),
        sf::Vector2f(std::round(midpoint.x + 0), std::round(midpoint.y - m_arrow_size / 2)),
        sf::Vector2f(std::round(midpoint.x + m_arrow_size / 2), std::round(midpoint.y - m_arrow_size / 2)),
        sf::Vector2f(std::round(midpoint.x - m_arrow_size / 2), std::round(midpoint.y + 0)),
        sf::Vector2f(std::round(midpoint.x + m_arrow_size / 2), std::round(midpoint.y + 0)),
        sf::Vector2f(std::round(midpoint.x - m_arrow_size / 2), std::round(midpoint.y + m_arrow_size / 2)),
        sf::Vector2f(std::round(midpoint.x + 0), std::round(midpoint.y + m_arrow_size / 2)),
        sf::Vector2f(std::round(midpoint.x + m_arrow_size / 2), std::round(midpoint.y + m_arrow_size / 2))
    };

    sf::VertexArray arrow(sf::Triangles, 3);
    switch (m_arrow_type) {
    case ArrowType::LEFTARROW:
        arrow[0] = sf::Vertex(points[2], m_arrow_color);
        arrow[1] = sf::Vertex(points[3], m_arrow_color);
        arrow[2] = sf::Vertex(points[7], m_arrow_color);
        break;
    case ArrowType::BOTTOMARROW:
        arrow[0] = sf::Vertex(points[0], m_arrow_color);
        arrow[1] = sf::Vertex(points[6], m_arrow_color);
        arrow[2] = sf::Vertex(points[2], m_arrow_color);
        break;
    case ArrowType::RIGHTARROW:
        arrow[0] = sf::Vertex(points[0], m_arrow_color);
        arrow[1] = sf::Vertex(points[4], m_arrow_color);
        arrow[2] = sf::Vertex(points[5], m_arrow_color);
        break;
    case ArrowType::TOPARROW:
        arrow[0] = sf::Vertex(points[5], m_arrow_color);
        arrow[1] = sf::Vertex(points[1], m_arrow_color);
        arrow[2] = sf::Vertex(points[7], m_arrow_color);
        break;
    }
    window.draw(arrow);
}

}