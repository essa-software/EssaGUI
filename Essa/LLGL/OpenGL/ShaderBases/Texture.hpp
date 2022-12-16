#pragma once

#include <Essa/LLGL/OpenGL/Shader.hpp>

namespace llgl::ShaderBases {

struct Texture {
    void set_texture(llgl::Texture const* tex) {
        m_texture.texture = tex;
        m_texture_set = tex;
    }

    static auto mapping() {
        return llgl::make_uniform_mapping(
            std::pair { "texture", &Texture::m_texture },
            std::pair { "textureSet", &Texture::m_texture_set });
    }

private:
    llgl::TextureUnit m_texture;
    bool m_texture_set = false;
};

}
