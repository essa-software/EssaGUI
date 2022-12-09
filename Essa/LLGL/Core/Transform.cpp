#include "Transform.hpp"

#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Vector.hpp>

namespace llgl {

Transform Transform::translate(Util::Vector3f vector) const {
    Util::Matrix4x4f translation_matrix {
        1, 0, 0, vector.x(),
        0, 1, 0, vector.y(),
        0, 0, 1, vector.z(),
        0, 0, 0, 1
    };
    return Transform { m_matrix * translation_matrix };
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
    Util::Matrix4x4f scale_matrix {
        scale, 0, 0, 0,
        0, scale, 0, 0,
        0, 0, scale, 0,
        0, 0, 0, 1
    };
    return Transform { m_matrix * scale_matrix };
}

Transform Transform::scale_x(float scale) const {
    Util::Matrix4x4f scale_matrix {
        scale, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return Transform { m_matrix * scale_matrix };
}

Transform Transform::scale_y(float scale) const {
    Util::Matrix4x4f scale_matrix {
        1, 0, 0, 0,
        0, scale, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return Transform { m_matrix * scale_matrix };
}

Transform Transform::scale_z(float scale) const {
    Util::Matrix4x4f scale_matrix {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, scale, 0,
        0, 0, 0, 1
    };
    return Transform { m_matrix * scale_matrix };
}

Util::Vector3f Transform::transform_point(Util::Vector3f const& vector) const {
    // TODO: Improve this constructor so that you can say vec4f(vector, 1) or something like this
    auto vector4 = Util::Vector4f { vector, 1 };
    if (m_matrix == Util::Matrix4x4f::identity())
        return vector;
    auto result = m_matrix * vector4;
    result /= result.w();
    return Util::Vector3f { result.x(), result.y(), result.z() };
}

}
