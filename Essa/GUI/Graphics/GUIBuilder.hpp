#pragma once

#include <Essa/GUI/Graphics/DefaultGUIShader.hpp>
#include <Essa/GUI/Graphics/Vertex.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/Builder.hpp>
#include <Essa/LLGL/OpenGL/DynamicShader.hpp>
#include <Essa/LLGL/OpenGL/MappedVertex.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Any.hpp>
#include <vector>

namespace Gfx {

struct ShaderContext {
    llgl::DynamicShader<Gfx::Vertex> shader;
    Util::AnyWithBase<Gfx::DefaultGUIShader::Uniforms> uniforms;
};

struct GUIBuilderRenderRange {
    llgl::PrimitiveType type;
    llgl::Projection projection;
    llgl::Transform view;
    llgl::Transform model;
    llgl::Transform submodel;
    llgl::Texture const* texture = nullptr;
    std::optional<ShaderContext> shader_context {};
};

// Builder optimized for generating 2D GUI components, widgets etc.
class GUIBuilder : public llgl::Builder<Vertex, GUIBuilderRenderRange> {
public:
    void add_vertices(std::span<Gfx::DefaultGUIShader::Vertex const> vertices, GUIBuilderRenderRange range) {
        add(vertices);
        add_render_range_for_last_vertices(vertices.size(), std::move(range));
    }

    virtual void render_range(
        llgl::Renderer& renderer, llgl::VertexArray<Vertex> const& vao, llgl::RenderRange<GUIBuilderRenderRange> range
    ) const override;

private:
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::create_vertex;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add;
    using llgl::Builder<Vertex, GUIBuilderRenderRange>::add_render_range_for_last_vertices;

    mutable Gfx::DefaultGUIShader m_shader;
};

}
