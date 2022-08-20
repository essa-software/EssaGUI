#include "RenderToTexture.hpp"

#include <LLGL/Renderer/StateScope.hpp>
#include <LLGL/Window/Window.hpp>

namespace llgl {

RenderToTexture::RenderToTexture(Window const& window)
    : m_window(&window)
    , m_fbo(window.size()) {
}

RenderToTexture::RenderToTexture(Util::Vector2i size)
    : m_fbo(size) {
}

void RenderToTexture::clear(std::optional<Util::Color> color) {
    opengl::FBOScope fbo_scope(m_fbo);
    if (m_window)
        m_fbo.resize(m_window->size());
    Renderer::clear(color);
}

void RenderToTexture::apply_projection(Projection const& p) {
    m_projection = p;
    opengl::set_viewport(p.viewport());
}

Projection RenderToTexture::projection() const {
    return m_projection;
}

void RenderToTexture::draw_vao(opengl::VAO const& vao, opengl::PrimitiveType primitive_type, DrawState const& state) {
    opengl::FBOScope fbo_scope(m_fbo);
    if (m_window)
        m_fbo.resize(m_window->size());
    StateScope scope(state, m_projection);
    vao.draw(state.shader->attribute_mapping(), primitive_type);
}

void RenderToTexture::set_label(std::string const& label) {
    m_fbo.set_label("RenderToTexture: " + label);
}

}
