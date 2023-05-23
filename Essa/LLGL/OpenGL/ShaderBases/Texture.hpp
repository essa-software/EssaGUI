#pragma once

#include <Essa/LLGL/OpenGL/Shader.hpp>

namespace llgl::ShaderBases {

struct Texture {
private:
    llgl::TextureUnit m_texture;
    bool m_texture_set = false;

public:
    void set_texture(llgl::Texture const* tex) {
        m_texture.texture = tex;
        m_texture_set = tex;
    }

    static inline auto mapping = llgl::make_uniform_mapping(
        llgl::Uniform { "texture", &Texture::m_texture }, llgl::Uniform { "textureSet", &Texture::m_texture_set }
    );
};

}
