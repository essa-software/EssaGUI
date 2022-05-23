#include "ArrowButton.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

namespace GUI {

void ArrowButton::draw(GUI::SFMLWindow& window) const {
    RectangleDrawOptions options;
    options.fill_color = bg_color_for_state();
    window.draw_rectangle({{}, size()}, options);

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

    std::array<Vertex, 3> arrow;
    switch (m_arrow_type) {
    case ArrowType::LEFTARROW:
        arrow[0] = Vertex { .position = Vector3(points[2]), .color = m_arrow_color };
        arrow[1] = Vertex { .position = Vector3(points[3]), .color = m_arrow_color };
        arrow[2] = Vertex { .position = Vector3(points[7]), .color = m_arrow_color };
        break;
    case ArrowType::BOTTOMARROW:
        arrow[0] = Vertex { .position = Vector3(points[0]), .color = m_arrow_color };
        arrow[1] = Vertex { .position = Vector3(points[6]), .color = m_arrow_color };
        arrow[2] = Vertex { .position = Vector3(points[2]), .color = m_arrow_color };
        break;
    case ArrowType::RIGHTARROW:
        arrow[0] = Vertex { .position = Vector3(points[0]), .color = m_arrow_color };
        arrow[1] = Vertex { .position = Vector3(points[4]), .color = m_arrow_color };
        arrow[2] = Vertex { .position = Vector3(points[5]), .color = m_arrow_color };
        break;
    case ArrowType::TOPARROW:
        arrow[0] = Vertex { .position = Vector3(points[5]), .color = m_arrow_color };
        arrow[1] = Vertex { .position = Vector3(points[1]), .color = m_arrow_color };
        arrow[2] = Vertex { .position = Vector3(points[7]), .color = m_arrow_color };
        break;
    }
    window.draw_vertices(GL_TRIANGLES, arrow);
}

}