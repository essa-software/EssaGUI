#pragma once

#include "Vertex.hpp"

#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Texture.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Transform.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>

namespace Gfx {

class DefaultGUIShader : public llgl::Shader {
public:
    using Vertex = Gfx::Vertex;

    struct Uniforms : public llgl::ShaderBases::Texture
        , public llgl::ShaderBases::Transform {
        static inline auto mapping = llgl::ShaderBases::Texture::mapping | llgl::ShaderBases::Transform::mapping;
    };

    std::string_view source(llgl::ShaderType type) const;
};

}
