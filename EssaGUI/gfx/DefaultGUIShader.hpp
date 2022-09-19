#pragma once

#include "Vertex.hpp"

#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/ShaderBases/Texture.hpp>
#include <LLGL/OpenGL/ShaderBases/Transform.hpp>
#include <LLGL/OpenGL/Vertex.hpp>

namespace Gfx {

class DefaultGUIShader : public llgl::Shader
    , public llgl::ShaderBases::Texture
    , public llgl::ShaderBases::Transform {
public:
    using Vertex = Gfx::Vertex;

    auto uniforms() {
        return llgl::UniformList { m_projection_matrix } + llgl::ShaderBases::Texture::uniforms();
    }

    void set_projection_matrix(Util::Matrix4x4f mat) { m_projection_matrix = mat; }

    std::string_view source(llgl::ShaderType type) const;

private:
    llgl::Uniform<Util::Matrix4x4f> m_projection_matrix { "projectionMatrix" };
};

}
