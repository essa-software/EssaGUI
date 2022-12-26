#pragma once

#include "../Shader.hpp"
#include <EssaUtil/Matrix.hpp>

namespace llgl::ShaderBases {

struct Transform {
private:
    Util::Matrix4x4f m_model_matrix;
    Util::Matrix4x4f m_view_matrix;
    Util::Matrix4x4f m_projection_matrix;

public:
    void set_transform(Util::Matrix4x4f model, Util::Matrix4x4f view, Util::Matrix4x4f projection) {
        m_model_matrix = model;
        m_view_matrix = view;
        m_projection_matrix = projection;
    }

    void set_model(Util::Matrix4x4f m) { m_model_matrix = m; }
    void set_view(Util::Matrix4x4f m) { m_view_matrix = m; }
    void set_projection(Util::Matrix4x4f m) { m_projection_matrix = m; }

    static inline auto mapping = llgl::make_uniform_mapping(
        llgl::Uniform { "modelMatrix", &Transform::m_model_matrix },
        llgl::Uniform { "viewMatrix", &Transform::m_view_matrix },
        llgl::Uniform { "projectionMatrix", &Transform::m_projection_matrix });

    auto model() const { return m_model_matrix; }
    auto view() const { return m_view_matrix; }
    auto projection() const { return m_projection_matrix; }
};

}
