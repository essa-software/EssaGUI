#pragma once

#include <Essa/LLGL/OpenGL/Shader.hpp>

namespace llgl::ShaderBases {

class Texture {
public:
    void set_texture(llgl::Texture const* tex) {
        m_texture->texture = tex;
        m_texture_set = tex;
    }

    auto uniforms() {
        return llgl::UniformList {
            m_texture,
            m_texture_set
        };
    }

private:
    llgl::Uniform<Util::Matrix4x4f> m_model_matrix { "modelMatrix" };
    llgl::Uniform<Util::Matrix4x4f> m_view_matrix { "viewMatrix" };
    llgl::Uniform<Util::Matrix4x4f> m_projection_matrix { "projectionMatrix" };
    llgl::Uniform<llgl::TextureUnit> m_texture { "texture", { 0, nullptr } };
    llgl::Uniform<bool> m_texture_set { "textureSet" };
};

}
