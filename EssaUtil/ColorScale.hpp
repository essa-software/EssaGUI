#pragma once

#include "Color.hpp"
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <ranges>

namespace Util {

class ColorScale {
public:
    struct Point {
        float position;
        Util::Color color;
    };

    ColorScale(std::initializer_list<Point> p)
        : m_points(p) {
        std::ranges::sort(m_points, [](auto const& l, auto const& r) {
            return l.position < r.position;
        });
    }

    Util::Color get(float position) const {
        assert(!m_points.empty());
        if (position <= m_points.front().position) {
            return m_points.front().color;
        }
        for (size_t s = 0; s < m_points.size() - 1; s++) {
            auto l = m_points[s];
            auto r = m_points[s + 1];
            if (position >= l.position && position <= r.position) {
                float normalized = (position - l.position) / (r.position - l.position);
                return Color::rgb_blend(l.color, r.color, normalized);
            }
        }
        return m_points.back().color;
    }

private:
    std::vector<Point> m_points;
};

}
