#pragma once

#include "../Vector.hpp"
#include "Plane.hpp"
#include <optional>

namespace Util::Math {

class Ray {
public:
    Ray(Util::Vector3d start, Util::Vector3d end)
        : m_start(start)
        , m_end(end) { }

    Util::Vector3d start() const { return m_start; }
    Util::Vector3d end() const { return m_end; }

    double distance_to_point(Util::Vector3d) const;
    std::optional<Util::Vector3d> intersection_with_plane(Plane const& plane) const;

private:
    Util::Vector3d m_start;
    Util::Vector3d m_end;
};

}
