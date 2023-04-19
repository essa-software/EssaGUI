#pragma once

#include "../Matrix.hpp"
#include "../Vector.hpp"

namespace Util::Math {

// Ax + By + Cz + D = 0
// FIXME: This belongs to LLGL or EssaUtil.
class Plane {
public:
    Plane(double a, double b, double c, double d)
        : m_a(a)
        , m_b(b)
        , m_c(c)
        , m_d(d) { }

    Plane(Util::Vector3d p1, Util::Vector3d p2, Util::Vector3d p3);

    double a() const { return m_a; }
    double b() const { return m_b; }
    double c() const { return m_c; }
    double d() const { return m_d; }

    Util::Vector3d normal() const;

    // *one of points that is known to be on the plane
    Util::Vector3d point() const;

    Plane transformed(Util::Matrix4x4d const&) const;

private:
    friend std::ostream& operator<<(std::ostream& out, Plane const&);

    double m_a = 0;
    double m_b = 0;
    double m_c = 0;
    double m_d = 0;
};

}
