#pragma once

#include <SFML/Graphics.hpp>

class RoundedEdgeRectangleShape : public sf::Shape {
public:
    explicit RoundedEdgeRectangleShape(sf::Vector2f size, float borderRadius)
        : m_size(size)
        , m_border_radius_top_left(borderRadius)
        , m_border_radius_top_right(borderRadius)
        , m_border_radius_bottom_left(borderRadius)
        , m_border_radius_bottom_right(borderRadius) { update(); }

    virtual std::size_t getPointCount() const override;
    virtual sf::Vector2f getPoint(std::size_t index) const override;
    sf::Vector2f getSize() const { return m_size; }

    void set_border_radius_top_left(float v) { m_border_radius_top_left = v; update(); }
    void set_border_radius_top_right(float v) { m_border_radius_top_right = v; update(); }
    void set_border_radius_bottom_left(float v) { m_border_radius_bottom_left = v; update(); }
    void set_border_radius_bottom_right(float v) { m_border_radius_bottom_right = v; update(); }

private:
    sf::Vector2f m_size;
    float m_border_radius_top_left = 0;
    float m_border_radius_top_right = 0;
    float m_border_radius_bottom_left = 0;
    float m_border_radius_bottom_right = 0;
};
