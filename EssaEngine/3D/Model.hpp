#pragma once

#include "Material.hpp"
#include <EssaGUI/gfx/ResourceManager.hpp>
#include <LLGL/Core/Vertex.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/Renderer/Renderable.hpp>
#include <LLGL/Renderer/Renderer.hpp>
#include <LLGL/Renderer/Transform.hpp>
#include <vector>

namespace Essa {

class Model : public llgl::Renderable {
public:
    struct Face {
        llgl::Vertex v1, v2, v3;
    };

    void add_face(Face face);
    void add_face(std::span<llgl::Vertex>);
    void set_material(Material mat) { m_material = std::move(mat); }

private:
    virtual void render(llgl::Renderer& renderer, llgl::DrawState) const override;

    std::optional<Material> m_material;
    std::vector<llgl::Vertex> m_vertexes;
    std::vector<llgl::Vertex> m_normal_vertexes;
    mutable llgl::opengl::VAO m_vao;
    mutable llgl::opengl::VAO m_normals_vao;
    mutable bool m_needs_update { true };
};

}

template<>
struct Gfx::ResourceTraits<Essa::Model> {
    static std::optional<Essa::Model> load_from_file(std::string const&);
    static std::string_view base_path() { return "models"; }
};