#pragma once

#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Graphics/ShaderResource.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Texture.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Transform.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <Essa/LLGL/OpenGL/VertexMapping.hpp>

namespace Essa::Shaders {

class Basic {
public:
    using Vertex = llgl::Vertex<Util::Point3f, Util::Colorf, Util::Point2f>;

    struct Uniforms
        : public llgl::ShaderBases::Transform
        , public llgl::ShaderBases::Texture {
        static inline auto mapping = llgl::ShaderBases::Transform::mapping | llgl::ShaderBases::Texture::mapping;
    };

    static Gfx::FullShaderResource<Basic>& load(Gfx::ResourceManager&);
};

}

template<>
struct llgl::VertexMapping<Essa::Shaders::Basic::Vertex> {
    static constexpr size_t position = 0;
    static constexpr size_t color = 1;
    static constexpr size_t tex_coord = 2;
};
