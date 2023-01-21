#include "Transform.hpp"

#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Vector.hpp>
#include <fmt/ostream.h>

namespace llgl {

Transform Transform::translate(Util::Vector3f vector) const {
    auto matrix = m_matrix;
    matrix.element(0, 3) = m_matrix.element(0, 0) * vector.x()
        + m_matrix.element(0, 1) * vector.y()
        + m_matrix.element(0, 2) * vector.z()
        + m_matrix.element(0, 3);
    matrix.element(1, 3) = m_matrix.element(1, 0) * vector.x()
        + m_matrix.element(1, 1) * vector.y()
        + m_matrix.element(1, 2) * vector.z()
        + m_matrix.element(1, 3);
    matrix.element(2, 3) = m_matrix.element(2, 0) * vector.x()
        + m_matrix.element(2, 1) * vector.y()
        + m_matrix.element(2, 2) * vector.z()
        + m_matrix.element(2, 3);
    matrix.element(3, 3) = m_matrix.element(3, 0) * vector.x()
        + m_matrix.element(3, 1) * vector.y()
        + m_matrix.element(3, 2) * vector.z()
        + m_matrix.element(3, 3);

    return Transform { matrix };
}

Transform Transform::rotate_x(float angle) const {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    Util::Matrix4x4f rotation_matrix {
        1, 0, 0, 0,
        0, std::cos(angle), std::sin(angle), 0,
        0, -std::sin(angle), std::cos(angle), 0,
        0, 0, 0, 1
    };
    return Transform { m_matrix * rotation_matrix };
}

Transform Transform::rotate_y(float angle) const {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    Util::Matrix4x4f rotation_matrix {
        std::cos(angle), 0, std::sin(angle), 0,
        0, 1, 0, 0,
        -std::sin(angle), 0, std::cos(angle), 0,
        0, 0, 0, 1
    };
    return Transform { m_matrix * rotation_matrix };
}

Transform Transform::rotate_z(float angle) const {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    Util::Matrix4x4f rotation_matrix {
        std::cos(angle), -std::sin(angle), 0, 0,
        std::sin(angle), std::cos(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return Transform { m_matrix * rotation_matrix };
}

Transform Transform::scale(float scale) const {
    auto matrix = m_matrix;
    for (size_t x = 0; x < 4; x++) {
        for (size_t y = 0; y < 3; y++) {
            matrix.element(x, y) *= scale;
        }
    }
    return Transform { matrix };
}

Transform Transform::scale_x(float scale) const {
    auto matrix = m_matrix;
    for (size_t x = 0; x < 4; x++) {
        matrix.element(x, 0) *= scale;
    }
    return Transform { matrix };
}

Transform Transform::scale_y(float scale) const {
    auto matrix = m_matrix;
    for (size_t x = 0; x < 4; x++) {
        matrix.element(x, 1) *= scale;
    }
    return Transform { matrix };
}

Transform Transform::scale_z(float scale) const {
    auto matrix = m_matrix;
    for (size_t x = 0; x < 4; x++) {
        matrix.element(x, 2) *= scale;
    }
    return Transform { matrix };
}

Util::Vector3f Transform::transform_point(Util::Vector3f const& vector) const {
    auto vector4 = Util::Vector4f { vector, 1 };
    if (m_matrix == Util::Matrix4x4f::identity())
        return vector;
    auto result = m_matrix * vector4;
    result /= result.w();
    return Util::Vector3f { result.x(), result.y(), result.z() };
}

Util::Vector2f Transform::transform_point_2d(Util::Vector2f const& vec) const {
    return Util::Vector2f { transform_point(Util::Vector3f { vec, 0 }) };
}

std::vector<Util::Vector3f> Transform::transform_points(std::vector<Util::Vector3f> const& points) const {
    std::vector<Util::Vector3f> output;
    for (auto const& p : points) {
        output.push_back(transform_point(p));
    }
    return output;
}

std::vector<Util::Vector2f> Transform::transform_points_2d(std::vector<Util::Vector2f> const& points) const {
    std::vector<Util::Vector2f> output;
    for (auto const& p : points) {
        output.push_back(transform_point_2d(p));
    }
    return output;
}

}
