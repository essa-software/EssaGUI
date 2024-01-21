#pragma once

#include <EssaUtil/CoordinateSystem.hpp>
#include <cassert>

namespace Eg {

// line in the form ax + by + c = 0
struct LineAxByC {
    float a;
    float b;
    float c;
};

// line in the form ax + b = y
struct LineAxB {
    float a;
    float b;
};

class Line2D {
public:
    static constexpr Line2D from_axbyc(LineAxByC abc) { return Line2D(abc); }
    static constexpr Line2D from_axb(LineAxB ab) { return Line2D({ ab.a, -1, ab.b }); }

    // construct line x = a
    static constexpr Line2D vertical(float x) { return Line2D({ 1, 0, -x }); }

    // construct line y = a
    static constexpr Line2D horizontal(float y) { return Line2D({ 0, 1, -y }); }

    // construct line through 2 points
    static Line2D from_points(Util::Point2f, Util::Point2f);

    constexpr bool is_vertical() const { return m_b == 0; }
    constexpr bool is_horizontal() const { return m_a == 0; }

    // get dy/dx
    constexpr float dydx() const {
        assert(!is_vertical());
        return -m_a / m_b;
    }
    // get dx/dy
    constexpr float dxdy() const {
        assert(!is_horizontal());
        return -m_b / m_a;
    }

    constexpr float y_intercept() const {
        assert(!is_vertical());
        return -m_c / m_b;
    }
    constexpr float x_intercept() const {
        assert(!is_horizontal());
        return -m_c / m_a;
    }

    constexpr LineAxByC to_axbyc() const { return { m_a, m_b, m_c }; }
    constexpr LineAxB to_axb() const { return { dydx(), y_intercept() }; }

private:
    constexpr Line2D(LineAxByC abc)
        : m_a(abc.a)
        , m_b(abc.b)
        , m_c(abc.c) {
        assert(abc.a != 0 || abc.b != 0);
    }

    // Internally we use a robust against vertical lines ax + by + c = 0 format
    float m_a;
    float m_b;
    float m_c;
};

}
