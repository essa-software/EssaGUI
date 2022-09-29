#pragma once

#include "../Shader.hpp"
#include <EssaUtil/Matrix.hpp>

namespace llgl::ShaderBases {

class Transform {
public:
    void set_transform(Util::Matrix4x4f model, Util::Matrix4x4f view, Util::Matrix4x4f projection) {
        m_model_matrix = model;
        m_view_matrix = view;
        m_projection_matrix = projection;
    }

    void set_model(Util::Matrix4x4f m) { m_model_matrix = m; }
    void set_view(Util::Matrix4x4f m) { m_view_matrix = m; }
    void set_projection(Util::Matrix4x4f m) { m_projection_matrix = m; }

    auto uniforms() {
        return llgl::UniformList {
            m_model_matrix,
            m_view_matrix,
            m_projection_matrix,
        };
    }

private:
    llgl::Uniform<Util::Matrix4x4f> m_model_matrix { "modelMatrix" };
    llgl::Uniform<Util::Matrix4x4f> m_view_matrix { "viewMatrix" };
    llgl::Uniform<Util::Matrix4x4f> m_projection_matrix { "projectionMatrix" };
};

}
