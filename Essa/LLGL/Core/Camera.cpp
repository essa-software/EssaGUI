#include "Camera.hpp"

#include <Essa/LLGL/Core/Transform.hpp>

namespace llgl {

Camera Camera::translate(Util::Vector3f vector) const {
    return Camera { m_projection, llgl::Transform {}.translate(-vector).matrix() * m_view_matrix };
}

Camera Camera::rotate_x(Util::Angle angle) const {
    return Camera { m_projection, llgl::Transform {}.rotate_x(-angle.rad()).matrix() * m_view_matrix };
    return *this;
}

Camera Camera::rotate_y(Util::Angle angle) const {
    return Camera { m_projection, llgl::Transform {}.rotate_y(-angle.rad()).matrix() * m_view_matrix };
    return *this;
}

Camera Camera::rotate_z(Util::Angle angle) const {
    return Camera { m_projection, llgl::Transform {}.rotate_z(-angle.rad()).matrix() * m_view_matrix };
    return *this;
}

}
