#pragma once

#include <Essa/Geometry/Line2D.hpp>
#include <EssaUtil/CoordinateSystem.hpp>
#include <EssaUtil/Rect.hpp>

namespace Eg {

class LineSegment2D {
public:
    constexpr LineSegment2D(Util::Point2f p1, Util::Point2f p2)
        : m_point1(p1)
        , m_point2(p2) { }

    constexpr Util::Point2f const& point1() const { return m_point1; }
    constexpr Util::Point2f const& point2() const { return m_point2; }
    // FIXME: constexpr
    Util::Point2f midpoint() const { return ((m_point1.to_vector() + m_point2.to_vector()) / 2).to_point(); }
    Util::Rectf bounding_box() const { return Util::Rectf::from_points(m_point1, m_point2); }

    // FIXME: constexpr
    Line2D extension() const { return Line2D::from_points(m_point1, m_point2); }
    double length_squared() const { return m_point1.distance_squared(m_point2); }
    double length() const { return m_point1.distance(m_point2); }

private:
    Util::Point2f m_point1;
    Util::Point2f m_point2;
};

}
