#pragma once

#include <EssaUtil/Matrix.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <cassert>

namespace llgl {

class View {
public:
    enum class Type {
        Ortho,
        Perspective
    };

    void set_viewport(Util::Recti viewport) { m_viewport = viewport; }
    void set_ortho(opengl::OrthoArgs ortho) { m_type = Type::Ortho, m_data.ortho = ortho; }
    void set_perspective(opengl::PerspectiveArgs persp) { m_type = Type::Perspective, m_data.perspective = persp; }

    Type type() const { return m_type; }
    Util::Recti viewport() const { return m_viewport; }
    opengl::OrthoArgs ortho_args() const { return m_data.ortho; }
    opengl::PerspectiveArgs perspective_args() const { return m_data.perspective; }

    Util::Matrix4x4f matrix() const;

private:
    Type m_type;
    Util::Recti m_viewport;

    union {
        opengl::OrthoArgs ortho;
        opengl::PerspectiveArgs perspective;
    } m_data;
};

}
