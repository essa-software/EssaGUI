#pragma once

#include "Vertex.hpp"

#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Texture.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Transform.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>

namespace Gfx {

class DefaultGUIShader : public llgl::Shader
    , public llgl::ShaderBases::Texture
    , public llgl::ShaderBases::Transform {
public:
    using Vertex = Gfx::Vertex;

    auto uniforms() {
        return llgl::ShaderBases::Transform::uniforms() + llgl::ShaderBases::Texture::uniforms();
    }

    std::string_view source(llgl::ShaderType type) const;
};

}
