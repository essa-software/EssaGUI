#pragma once

#include <EssaUtil/Color.hpp>

namespace Gfx::Drawing {

class Outline {
public:
    struct RoundRadius {
        float top_left = 0;
        float top_right = 0;
        float bottom_left = 0;
        float bottom_right = 0;
    };

    static Outline none() { return Outline::normal(Util::Colors::Transparent, 0); }

    static Outline normal(Util::Color color, float thickness) {
        return Outline {}.set_color(color).set_thickness(thickness);
    }
    static Outline rounded(Util::Color color, float thickness, float radius) {
        return Outline::normal(color, thickness).set_round_radius(radius);
    }
    static Outline rounded(Util::Color color, float thickness, RoundRadius radius) {
        return Outline::normal(color, thickness).set_round_radius(radius);
    }

    Outline& set_color(Util::Color color) {
        m_color = color;
        return *this;
    }
    Outline& set_thickness(float thickness) {
        m_thickness = thickness;
        return *this;
    }
    Outline& set_round_radius(float br) {
        return set_round_radius({ br, br, br, br });
    }
    Outline& set_round_radius(RoundRadius rr) {
        m_round_radius = rr;
        return *this;
    }

    Util::Color color() const { return m_color; };
    float thickness() const { return m_thickness; };
    RoundRadius round_radius() const { return m_round_radius; }

    bool is_rounded() const {
        return m_round_radius.top_left != 0 || m_round_radius.top_right != 0 || m_round_radius.bottom_left != 0 || m_round_radius.bottom_right != 0;
    }
    bool is_visible() const {
        return m_color.a > 0 && m_thickness != 0;
    }

private:
    Util::Color m_color;
    float m_thickness = 1;
    RoundRadius m_round_radius;
};

}
