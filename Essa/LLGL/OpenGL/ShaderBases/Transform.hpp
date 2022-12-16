#pragma once

#include "../Shader.hpp"
#include <EssaUtil/Matrix.hpp>

namespace llgl::ShaderBases {

struct Transform {
    void set_transform(Util::Matrix4x4f model, Util::Matrix4x4f view, Util::Matrix4x4f projection) {
        m_model_matrix = model;
        m_view_matrix = view;
        m_projection_matrix = projection;
    }

    void set_model(Util::Matrix4x4f m) { m_model_matrix = m; }
    void set_view(Util::Matrix4x4f m) { m_view_matrix = m; }
    void set_projection(Util::Matrix4x4f m) { m_projection_matrix = m; }

    static auto mapping() {
        return llgl::make_uniform_mapping(
            std::pair { "modelMatrix", &Transform::m_model_matrix },
            std::pair { "viewMatrix", &Transform::m_view_matrix },
            std::pair { "projectionMatrix", &Transform::m_projection_matrix });
    }

private:
    Util::Matrix4x4f m_model_matrix;
    Util::Matrix4x4f m_view_matrix;
    Util::Matrix4x4f m_projection_matrix;
};

}
