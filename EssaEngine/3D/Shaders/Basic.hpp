#pragma once

#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/ShaderBases/Texture.hpp>
#include <LLGL/OpenGL/ShaderBases/Transform.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/VertexMapping.hpp>

namespace Essa::Shaders {

class Basic : public llgl::Shader
    , public llgl::ShaderBases::Transform
    , public llgl::ShaderBases::Texture {
public:
    using Vertex = llgl::Vertex<Util::Vector3f, Util::Colorf, Util::Vector2f>;

    auto uniforms() {
        return llgl::ShaderBases::Transform::uniforms()
            + llgl::ShaderBases::Texture::uniforms();
    }

    std::string_view source(llgl::ShaderType type) const;
};

}

template<>
struct llgl::VertexMapping<Essa::Shaders::Basic::Vertex> {
    static constexpr size_t position = 0;
    static constexpr size_t color = 1;
    static constexpr size_t tex_coord = 2;
};
