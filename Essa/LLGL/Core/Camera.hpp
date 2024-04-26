#pragma once

#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Vector.hpp>

namespace llgl {

class Camera {
public:
    explicit Camera(llgl::Projection projection, Util::Matrix4x4f view_matrix = {})
        : m_projection(projection)
        , m_view_matrix(view_matrix) { }

    [[nodiscard]] Camera translate(Util::Vector3f vector) const;
    [[nodiscard]] Camera rotate_x(Util::Angle angle) const;
    [[nodiscard]] Camera rotate_y(Util::Angle angle) const;
    [[nodiscard]] Camera rotate_z(Util::Angle angle) const;

    llgl::Projection projection() const {
        return m_projection;
    };
    Util::Matrix4x4f view_matrix() const {
        return m_view_matrix;
    };

private:
    llgl::Projection m_projection;
    Util::Matrix4x4f m_view_matrix;
};

}
