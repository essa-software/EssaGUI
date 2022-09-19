#pragma once

#include "DrawState.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"

#include <LLGL/OpenGL/FBO.hpp>
#include <LLGL/OpenGL/Texture.hpp>

namespace llgl {

class Framebuffer {
public:
    explicit Framebuffer(Util::Vector2i size)
        : m_fbo(size)
        , m_renderer(m_fbo.id()) { }

    void clear() {
        m_renderer.clear();
    }

    template<class VertT, class DSS>
    requires(std::is_same_v<typename DSS::Vertex, VertT>) void draw_vertices(VertexArray<VertT> const& vbo, DrawState<DSS> const& draw_state) {
        m_renderer.draw_vertices(vbo, draw_state);
    }

    void resize(Util::Vector2i size) {
        m_fbo.resize(size);
    }

    void set_label(std::string const& label) {
        m_fbo.set_label(label);
    }

    auto const& color_texture() const { return m_fbo.color_texture(); }

private:
    llgl::opengl::FBO m_fbo;
    Renderer m_renderer;
};

}
