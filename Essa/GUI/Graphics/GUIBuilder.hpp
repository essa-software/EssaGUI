#pragma once

#include <Essa/GUI/Graphics/DefaultGUIShader.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/Builder.hpp>
#include <Essa/LLGL/OpenGL/MappedVertex.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <EssaUtil/Angle.hpp>
#include <vector>

namespace Gfx {

struct GUIBuilderRenderRange : public llgl::RenderRange {
    llgl::Projection projection;
    llgl::Transform view;
    llgl::Transform model;
    llgl::Transform submodel;
    llgl::Texture const* texture = nullptr;
};

// Builder optimized for generating 2D GUI components, widgets etc.
class GUIBuilder : public llgl::Builder<Vertex, GUIBuilderRenderRange> {
public:
    struct RegularPolygonDrawOptions {
        Util::Colorf color = Util::Colors::White;
        Util::Angle rotation {};
    };

    void add_vertices(llgl::PrimitiveType mode, std::span<Gfx::DefaultGUIShader::Vertex const> vertices, llgl::Texture const* texture) {
        for (auto const& v : vertices)
            add(v);
        add_render_range_for_last_vertices(vertices.size(), mode, m_projection, m_view, m_model, m_submodel, texture);
    }

    void set_projection(llgl::Projection projection) { m_projection = projection; }
    auto projection() const { return m_projection; }
    void set_view(llgl::Transform const& transform) { m_view = transform; }
    auto view() const { return m_view; }
    void set_model(llgl::Transform const& transform) { m_model = transform; }
    auto model() const { return m_model; }

    // Submodel should NEVER be accessed directly, only in Painter.
    // It is not guaranteed to maintain state after drawing calls.
    void set_submodel(llgl::Transform const& transform) { m_submodel = transform; }
    auto submodel() const { return m_submodel; }

private:
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::create_vertex;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add_render_range_for_last_vertices;

    virtual void
    render_range(llgl::Renderer& renderer, llgl::VertexArray<Vertex> const& vao, GUIBuilderRenderRange const& range) const override {
        Gfx::DefaultGUIShader::Uniforms uniforms;
        uniforms.set_transform(range.model.matrix(), range.view.matrix(), range.projection.matrix());
        uniforms.set_texture(range.texture);
        uniforms.submodel_matrix = range.submodel.matrix();
        auto viewport = range.projection.viewport();
        viewport.top = static_cast<int>(renderer.size().y()) - viewport.top - viewport.height;
        llgl::set_viewport(viewport);
        renderer.draw_vertices(vao, llgl::DrawState { m_shader, uniforms, range.type }, range.first, range.size);
    }

    mutable Gfx::DefaultGUIShader m_shader;
    llgl::Projection m_projection;
    llgl::Transform m_view;
    llgl::Transform m_model;
    llgl::Transform m_submodel;
};

}
