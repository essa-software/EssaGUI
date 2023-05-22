#pragma once

#include "../Vector.hpp"
#include "Plane.hpp"
#include <optional>

namespace Util::Math {

class Ray {
public:
    Ray(Util::Cs::Point3d start, Util::Cs::Point3d end)
        : m_start(start)
        , m_end(end) { }

    Util::Cs::Point3d start() const { return m_start; }
    Util::Cs::Point3d end() const { return m_end; }

    double distance_to_point(Util::Cs::Point3d) const;
    std::optional<Util::Cs::Point3d> intersection_with_plane(Plane const& plane) const;

private:
    Util::Cs::Point3d m_start;
    Util::Cs::Point3d m_end;
};

}
