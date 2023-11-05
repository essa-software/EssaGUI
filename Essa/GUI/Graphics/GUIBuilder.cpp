#include "GUIBuilder.hpp"

namespace Gfx {

void GUIBuilder::render_range(
    llgl::Renderer& renderer, llgl::VertexArray<Vertex> const& vao, llgl::RenderRange<GUIBuilderRenderRange> range
) const {
    auto& range_data = range.data;
    Gfx::DefaultGUIShader::Uniforms default_uniforms;

    auto& uniforms = range_data.shader_context ? range_data.shader_context->uniforms.ref() : default_uniforms;
    uniforms.set_transform(range_data.model.matrix(), range_data.view.matrix(), range_data.projection.matrix());
    uniforms.set_texture(range_data.texture);
    uniforms.submodel_matrix = range_data.submodel.matrix();

    auto viewport = range_data.projection.viewport();
    viewport.top = static_cast<int>(renderer.size().y()) - viewport.top - viewport.height;
    llgl::set_viewport(viewport);

    if (range_data.shader_context) {
        renderer.draw_vertices(
            vao,
            llgl::DrawState { range_data.shader_context->shader,
                              llgl::DynamicShader<Gfx::Vertex>::Uniforms { range_data.shader_context->uniforms }, range_data.type },
            range.first, range.size
        );
    }
    else {
        renderer.draw_vertices(vao, llgl::DrawState { m_shader, uniforms, range_data.type }, range.first, range.size);
    }
}

}
