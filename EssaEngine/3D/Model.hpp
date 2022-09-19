#pragma once

#include "Material.hpp"
#include "Shaders/Basic.hpp"

#include <EssaGUI/gfx/ResourceManager.hpp>
#include <LLGL/OpenGL/PrimitiveType.hpp>
#include <LLGL/OpenGL/Renderer.hpp>
#include <vector>

#define OBJECT3D_DEBUG 0

namespace Essa {

class Model {
public:
    // position (xyz), color (rgba), tex coord (st), normal (xyz)
    using Vertex = llgl::Vertex<Util::Vector3f, Util::Colorf, Util::Vector2f, Util::Vector3f>;

    struct Face {
        Vertex v1, v2, v3;
    };

    void add_face(Face face);
    void add_face(std::span<Vertex>);
    void set_material(Material mat) { m_material = std::move(mat); }

    void render(llgl::Renderer& renderer, llgl::ShaderImpl auto& shader) const {
        if (m_needs_update) {
            std::cerr << "needs update!" << std::endl;
            m_vao.upload_vertices(m_vertexes);
            if constexpr (OBJECT3D_DEBUG)
                m_normals_vao.upload_vertices(m_normal_vertexes);
            m_needs_update = false;
        }

        static Essa::Shaders::Basic basic_shader;

        if (m_material && m_material->diffuse.texture)
            shader.set_texture(m_material->diffuse.texture);

        renderer.draw_vertices(m_vao, llgl::DrawState { shader, llgl::PrimitiveType::Triangles });

        if constexpr (OBJECT3D_DEBUG)
            renderer.draw_vertices(m_normals_vao, llgl::DrawState { basic_shader, llgl::PrimitiveType::Triangles });
    }

private:
    std::optional<Material> m_material;
    std::vector<Vertex> m_vertexes;
    std::vector<Shaders::Basic::Vertex> m_normal_vertexes;
    mutable llgl::VertexArray<Vertex> m_vao;
    mutable llgl::VertexArray<Shaders::Basic::Vertex> m_normals_vao;
    mutable bool m_needs_update { true };
};

}

template<>
struct Gfx::ResourceTraits<Essa::Model> {
    static std::optional<Essa::Model> load_from_file(std::string const&);
    static std::string_view base_path() { return "models"; }
};
