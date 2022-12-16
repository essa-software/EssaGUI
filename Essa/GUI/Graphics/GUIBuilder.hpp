#pragma once

#include <Essa/GUI/Graphics/DefaultGUIShader.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/Builder.hpp>
#include <Essa/LLGL/OpenGL/MappedVertex.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <vector>

namespace Gfx {

struct GUIBuilderRenderRange : public llgl::RenderRange {
    llgl::Projection projection;
    llgl::Transform view;
    llgl::Transform model;
    llgl::Texture const* texture = nullptr;
};

// Builder optimized for generating 2D GUI components, widgets etc.
class GUIBuilder : public llgl::Builder<Vertex, GUIBuilderRenderRange> {
public:
    struct RectangleDrawOptions {
        llgl::Texture const* texture = nullptr;
        Util::Rectf texture_rect;
        Util::Colorf fill_color;
    };

    void add_rectangle(Util::Rectf const& rect, RectangleDrawOptions const& options) {
        add({
            create_vertex(Util::Vector3f { rect.left, rect.top, 0 }, options.fill_color,
                Util::Vector2f { options.texture_rect.left, options.texture_rect.top }, Util::Vector3f {}),
            create_vertex(Util::Vector3f { rect.left + rect.width, rect.top, 0 }, options.fill_color,
                Util::Vector2f { options.texture_rect.left + options.texture_rect.width, options.texture_rect.top }, Util::Vector3f {}),
            create_vertex(Util::Vector3f { rect.left, rect.top + rect.height, 0 }, options.fill_color,
                Util::Vector2f { options.texture_rect.left, options.texture_rect.top + options.texture_rect.height }, Util::Vector3f {}),
            create_vertex(Util::Vector3f { rect.left + rect.width, rect.top + rect.height, 0 }, options.fill_color,
                Util::Vector2f { options.texture_rect.left + options.texture_rect.width, options.texture_rect.top + options.texture_rect.height }, Util::Vector3f {}),
        });
        add_render_range_for_last_vertices(4, llgl::PrimitiveType::TriangleStrip, m_projection, m_view, m_model, options.texture);
    }

    void add_regular_polygon(Util::Vector2f center, float radius, size_t vertices, Util::Colorf const& color = Util::Colors::White) {
        for (size_t s = 0; s < vertices; s++) {
            float angle = M_PI * 2 * s / vertices;
            add(create_vertex(Util::Vector3f(Util::Vector2f::create_polar(angle, radius) + center, 0), color, Util::Vector2f {}, Util::Vector3f {}));
        }
        add(create_vertex(Util::Vector3f(Util::Vector2f { radius, 0 } + center, 0), color, Util::Vector2f {}, Util::Vector3f {}));
        add_render_range_for_last_vertices(vertices + 1, llgl::PrimitiveType::TriangleFan, m_projection, m_view, m_model);
    }

    void add_vertices(llgl::PrimitiveType mode, std::span<Gfx::DefaultGUIShader::Vertex const> vertices, llgl::Texture const* texture) {
        for (auto const& v : vertices)
            add(v);
        add_render_range_for_last_vertices(vertices.size(), mode, m_projection, m_view, m_model, texture);
    }

    void set_projection(llgl::Projection projection) { m_projection = std::move(projection); }
    auto projection() const { return m_projection; }
    void set_view(llgl::Transform const& transform) { m_view = transform; }
    auto view() const { return m_view; }
    void set_model(llgl::Transform const& transform) { m_model = transform; }
    auto model() const { return m_model; }

private:
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::create_vertex;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add_render_range_for_last_vertices;

    virtual void render_range(llgl::Renderer& renderer, llgl::VertexArray<Vertex> const& vao, GUIBuilderRenderRange const& range) const override {
        Gfx::DefaultGUIShader::Uniforms uniforms;
        uniforms.set_transform(range.model.matrix(), range.view.matrix(), range.projection.matrix());
        uniforms.set_texture(range.texture);
        llgl::set_viewport(range.projection.viewport());
        renderer.draw_vertices(vao, llgl::DrawState { m_shader, uniforms, range.type }, range.first, range.size);
    }

    mutable Gfx::DefaultGUIShader m_shader;
    llgl::Projection m_projection;
    llgl::Transform m_view;
    llgl::Transform m_model;
};

}
