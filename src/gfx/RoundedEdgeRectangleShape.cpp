#include "RoundedEdgeRectangleShape.hpp"

#include <cmath>
#include <iostream>

constexpr size_t ROUNDED_EDGE_RESOLUTION = 10;

std::size_t RoundedEdgeRectangleShape::getPointCount() const {
    return ROUNDED_EDGE_RESOLUTION * 4;
}

sf::Vector2f RoundedEdgeRectangleShape::getPoint(std::size_t index) const {
    size_t side = index / ROUNDED_EDGE_RESOLUTION;

    float subangle = (float)(index % ROUNDED_EDGE_RESOLUTION) / (ROUNDED_EDGE_RESOLUTION - 1) * M_PI_2;
    float angle = subangle + side * M_PI_2;

    float radius_top_left = std::min(std::min(m_border_radius_top_left, m_size.x / 2), m_size.y / 2);
    float radius_top_right = std::min(std::min(m_border_radius_top_right, m_size.x / 2), m_size.y / 2);
    float radius_bottom_left = std::min(std::min(m_border_radius_bottom_left, m_size.x / 2), m_size.y / 2);
    float radius_bottom_right = std::min(std::min(m_border_radius_bottom_right, m_size.x / 2), m_size.y / 2);

    sf::Vector2f offset;
    sf::Vector2f base;
    switch (side) {
    case 0:
        base = { m_size.x - radius_bottom_right, m_size.y - radius_bottom_right };
        offset = { std::cos(angle) * radius_bottom_right, std::sin(angle) * radius_bottom_right };
        break;
    case 1:
        base = { radius_bottom_left, m_size.y - radius_bottom_left };
        offset = { std::cos(angle) * radius_bottom_left, std::sin(angle) * radius_bottom_left };
        break;
    case 2:
        base = { radius_top_left, radius_top_left };
        offset = { std::cos(angle) * radius_top_left, std::sin(angle) * radius_top_left };
        break;
    case 3:
        base = { m_size.x - radius_top_right, radius_top_right };
        offset = { std::cos(angle) * radius_top_right, std::sin(angle) * radius_top_right };
        break;
    default:
        break;
    }
    auto result = base + offset;
    return result;
}
