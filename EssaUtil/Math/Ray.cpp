#include "Ray.hpp"

#include <iostream>

namespace Util::Math {

double Ray::distance_to_point(Util::Point3d point) const {
    // https://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
    double num = (point - m_start).cross(point - m_end).length();
    double den = (m_end - m_start).length();
    return num / den;
}

std::optional<Util::Point3d> Ray::intersection_with_plane(Plane const& plane) const {
    // https://stackoverflow.com/questions/5666222/3d-line-plane-intersection
    // TODO: Do ray&plane, not line&plane intersection
    auto plane_normal = plane.normal();
    auto plane_point = plane.point();
    plane_normal = plane_normal.normalized();

    auto line_direction = (m_start - m_end).normalized();
    if (plane_normal.dot(line_direction) == 0)
        return {};

    // std::cout << m_start << ":" << m_end << std::endl;

    double t = (plane_normal.dot(plane_point.to_vector()) - plane_normal.dot(m_start.to_vector())) / plane_normal.dot(line_direction);
    return m_start + line_direction * t;
}

}
