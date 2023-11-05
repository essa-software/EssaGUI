#include "GUIBuilder.hpp"

namespace Gfx {

void GUIBuilder::render_range(llgl::Renderer& renderer, llgl::VertexArray<Vertex> const& vao, GUIBuilderRenderRange const& r) const {
    auto range = r;
    Gfx::DefaultGUIShader::Uniforms default_uniforms;

    auto& uniforms = range.shader_context ? range.shader_context->uniforms.ref() : default_uniforms;
    uniforms.set_transform(range.model.matrix(), range.view.matrix(), range.projection.matrix());
    uniforms.set_texture(range.texture);
    uniforms.submodel_matrix = range.submodel.matrix();

    auto viewport = range.projection.viewport();
    viewport.top = static_cast<int>(renderer.size().y()) - viewport.top - viewport.height;
    llgl::set_viewport(viewport);

    if (range.shader_context) {
        renderer.draw_vertices(
            vao, llgl::DrawState { range.shader_context->shader, llgl::DynamicShader<Gfx::Vertex>::Uniforms { uniforms }, range.type },
            range.first, range.size
        );
    }
    else {
        renderer.draw_vertices(vao, llgl::DrawState { m_shader, uniforms, range.type }, range.first, range.size);
    }
}

}
