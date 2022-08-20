#include "CoreRenderer.hpp"

#include <EssaUtil/Matrix.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/Renderer/StateScope.hpp>
#include <LLGL/Window/Window.hpp>
#include <iostream>

namespace llgl {

void CoreRenderer::apply_projection(Projection const& projection)
{
    m_projection = projection;
    auto viewport_rect = projection.viewport();
    viewport_rect.top = m_window.size().y() - (viewport_rect.top + viewport_rect.height);
    opengl::set_viewport(viewport_rect);
}

Projection CoreRenderer::projection() const
{
    return m_projection;
}

void CoreRenderer::draw_vao(opengl::VAO const& vao, opengl::PrimitiveType primitive_type, DrawState const& state)
{
    StateScope scope(state, m_projection);
    vao.draw(state.shader->attribute_mapping(), primitive_type);
}

}
