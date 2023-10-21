#include "GUIBuilder.hpp"

namespace Gfx {

void GUIBuilder::render_range(llgl::Renderer& renderer, llgl::VertexArray<Vertex> const& vao, GUIBuilderRenderRange const& range) const {
    Gfx::DefaultGUIShader::Uniforms uniforms;
    uniforms.set_transform(range.model.matrix(), range.view.matrix(), range.projection.matrix());
    uniforms.set_texture(range.texture);
    uniforms.submodel_matrix = range.submodel.matrix();
    auto viewport = range.projection.viewport();
    viewport.top = static_cast<int>(renderer.size().y()) - viewport.top - viewport.height;
    llgl::set_viewport(viewport);
    renderer.draw_vertices(vao, llgl::DrawState { m_shader, uniforms, range.type }, range.first, range.size);
}

}
