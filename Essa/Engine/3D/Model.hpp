#pragma once

#include "Material.hpp"
#include "Shaders/Basic.hpp"
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/NonCopyable.hpp>
#include <Essa/LLGL/OpenGL/Builder.hpp>
#include <Essa/LLGL/OpenGL/PrimitiveType.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <functional>
#include <vector>

#define OBJECT3D_DEBUG 0

namespace Essa {

using ModelVertex = llgl::Vertex<Util::Vector3f, Util::Colorf, Util::Vector2f, Util::Vector3f>;

}

template<>
struct llgl::VertexMapping<Essa::ModelVertex> {
    static constexpr size_t position = 0;
    static constexpr size_t color = 1;
    static constexpr size_t tex_coord = 2;
    static constexpr size_t normal = 3;
};

namespace Essa {

struct ModelRenderRange : public llgl::RenderRange {
    std::optional<Material> material;
};

class ModelBuilder : public llgl::Builder<ModelVertex, ModelRenderRange> {
public:
    using RangeRenderer = std::function<void(llgl::Renderer& renderer, llgl::VertexArray<ModelVertex> const& vao, ModelRenderRange const& range)>;

    void set_range_renderer(RangeRenderer rr) { m_renderer = std::move(rr); }

    void add_range(std::span<ModelVertex const> vertices, std::optional<Material> material) {
        for (auto const& vertex : vertices) {
            add(vertex);
        }
        add_render_range_for_last_vertices(vertices.size(), llgl::PrimitiveType::Triangles, std::move(material));
    }

private:
    virtual void render_range(llgl::Renderer& renderer, llgl::VertexArray<ModelVertex> const& vao, ModelRenderRange const& range) const override {
        m_renderer(renderer, vao, range);
    }
    RangeRenderer m_renderer;
};

class Model : public Util::NonCopyable {
public:
    // position (xyz), color (rgba), tex coord (st), normal (xyz)
    using Vertex = ModelVertex;

    void add_range(std::span<ModelVertex const> vertices, std::optional<Material> material) {
        m_builder.add_range(vertices, material);
    }

    void render(llgl::Renderer& renderer, llgl::ShaderImpl auto& shader) const {
        auto range_renderer = [&shader](llgl::Renderer& renderer, llgl::VertexArray<ModelVertex> const& vao, ModelRenderRange const& range) {
            if constexpr (requires() { shader.set_material(*range.material); }) {
                if (range.material) {
                    shader.set_material(*range.material);
                }
                else {
                    shader.set_material(Material {
                        .ambient = { .color = Util::Colors::Gray },
                        .diffuse = { .color = Util::Colors::White },
                        .emission { .color = Util::Colors::Black },
                    });
                }
            }

            renderer.draw_vertices(vao, llgl::DrawState { shader, range.type }, range.first, range.size);
        };
        m_builder.set_range_renderer(std::move(range_renderer));
        m_builder.render(renderer);
    }

private:
    mutable ModelBuilder m_builder;
};

}

template<>
struct Gfx::ResourceTraits<Essa::Model> {
    static std::optional<Essa::Model> load_from_file(std::string const&);
    static std::string_view base_path() { return "models"; }
};
