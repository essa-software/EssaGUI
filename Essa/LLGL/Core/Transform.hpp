#pragma once

#include <EssaUtil/CoordinateSystem.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Vector.hpp>
#include <vector>

namespace llgl {

class Transform {
public:
    explicit Transform(Util::Matrix4x4f matrix = Util::Matrix4x4f::identity())
        : m_matrix(matrix) {
    }

    [[nodiscard]] Transform translate(Util::Cs::Vector3f const& by) const;

    // Angle is in radians.
    [[nodiscard]] Transform rotate_x(float angle) const;
    [[nodiscard]] Transform rotate_y(float angle) const;
    [[nodiscard]] Transform rotate_z(float angle) const;

    [[nodiscard]] Transform scale(float) const;
    [[nodiscard]] Transform scale_x(float) const;
    [[nodiscard]] Transform scale_y(float) const;
    [[nodiscard]] Transform scale_z(float) const;

    Util::Matrix4x4f matrix() const { return m_matrix; }

    Util::Cs::Point3f transform_point(Util::Cs::Point3f const&) const;

    // Transform a point using z = 0 and ignoring z on return.
    Util::Cs::Point2f transform_point_2d(Util::Cs::Point2f const&) const;

    std::vector<Util::Cs::Point3f> transform_points(std::vector<Util::Cs::Point3f> const&) const;
    std::vector<Util::Cs::Point2f> transform_points_2d(std::vector<Util::Cs::Point2f> const&) const;

private:
    Util::Matrix4x4f m_matrix = Util::Matrix4x4f::identity();
};

}
