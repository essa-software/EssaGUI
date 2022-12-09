#pragma once

#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <EssaUtil/Matrix.hpp>
#include <cassert>

namespace llgl {

class Projection {
public:
    static Projection ortho(OrthoArgs, Util::Recti viewport);
    static Projection perspective(PerspectiveArgs, Util::Recti viewport);

    Projection() = default;

    Projection(Util::Matrix4x4f matrix, Util::Recti viewport)
        : m_matrix(matrix)
        , m_viewport(viewport) { }

    Util::Recti viewport() const { return m_viewport; }
    Util::Matrix4x4f matrix() const { return m_matrix; }

    void set_viewport(Util::Recti v) { m_viewport = v; }

private:
    Util::Matrix4x4f m_matrix;
    Util::Recti m_viewport;
};

}
