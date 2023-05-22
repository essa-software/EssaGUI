#pragma once

#include "../Vector.hpp"
#include "Plane.hpp"
#include <optional>

namespace Util::Math {

class Ray {
public:
    Ray(Util::Point3d start, Util::Point3d end)
        : m_start(start)
        , m_end(end) { }

    Util::Point3d start() const { return m_start; }
    Util::Point3d end() const { return m_end; }

    double distance_to_point(Util::Point3d) const;
    std::optional<Util::Point3d> intersection_with_plane(Plane const& plane) const;

private:
    Util::Point3d m_start;
    Util::Point3d m_end;
};

}
