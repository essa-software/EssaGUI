#pragma once

#include <EssaGUI/gfx/DefaultGUIShader.hpp>
#include <LLGL/OpenGL/Builder.hpp>
#include <LLGL/OpenGL/MappedVertex.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/OpenGL/Renderer.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <LLGL/OpenGL/VertexArray.hpp>
#include <vector>

namespace Gfx {

struct GUIBuilderRenderRange : public llgl::RenderRange {
    llgl::Projection projection;
};

// Builder optimized for generating 2D GUI components, widgets etc.
class GUIBuilder : public llgl::Builder<Vertex, GUIBuilderRenderRange> {
public:
    void add_rectangle(Util::Rectf const& rect, Util::Colorf const& color = Util::Colors::White, Util::Rectf const& texture_rect = {}) {
        add({
            create_vertex(Util::Vector3f { rect.left, rect.top, 0 }, color,
                Util::Vector2f { texture_rect.left, texture_rect.top }, Util::Vector3f {}),
            create_vertex(Util::Vector3f { rect.left + rect.width, rect.top, 0 }, color,
                Util::Vector2f { texture_rect.left + texture_rect.width, texture_rect.top }, Util::Vector3f {}),
            create_vertex(Util::Vector3f { rect.left, rect.top + rect.height, 0 }, color,
                Util::Vector2f { texture_rect.left, texture_rect.top + texture_rect.height }, Util::Vector3f {}),
            create_vertex(Util::Vector3f { rect.left + rect.width, rect.top + rect.height, 0 }, color,
                Util::Vector2f { texture_rect.left + texture_rect.width, texture_rect.top + texture_rect.height }, Util::Vector3f {}),
        });
        add_render_range_for_last_vertices(4, llgl::PrimitiveType::TriangleStrip, m_projection);
    }

    void add_regular_polygon(Util::Vector2f center, float radius, size_t vertices, Util::Colorf const& color = Util::Colors::White) {
        for (size_t s = 0; s < vertices; s++) {
            float angle = 6.28 * s / vertices;
            Util::Vector2f vpos { radius * std::sin(angle), radius * std::cos(angle) };
            add(create_vertex(Util::Vector3f(vpos + center, 0), color, Util::Vector2f {}, Util::Vector3f {}));
        }
        add(create_vertex(Util::Vector3f(Util::Vector2f { 0, radius } + center, 0), color, Util::Vector2f {}, Util::Vector3f {}));
        add_render_range_for_last_vertices(vertices + 1, llgl::PrimitiveType::TriangleFan, m_projection);
    }

    void set_projection(llgl::Projection projection) { m_projection = std::move(projection); }

private:
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::create_vertex;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add_render_range_for_last_vertices;

    virtual void render_range(llgl::Renderer& renderer, llgl::VertexArray<Vertex> const& vao, GUIBuilderRenderRange const& range) const override {
        static Gfx::DefaultGUIShader shader;
        shader.set_projection_matrix(m_projection.matrix());
        llgl::set_viewport(m_projection.viewport());
        renderer.draw_vertices(vao, llgl::DrawState { shader, range.type }, range.first, range.size);
    }

    llgl::Projection m_projection;
};

}
