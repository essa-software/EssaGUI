#pragma once

#include <LLGL/OpenGL/DrawState.hpp>
#include <LLGL/OpenGL/FBO.hpp>
#include <LLGL/OpenGL/Renderer.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/VertexArray.hpp>

namespace llgl {

class Framebuffer {
public:
    explicit Framebuffer(Util::Vector2u size)
        : m_fbo(size)
        , m_renderer(m_fbo.id()) { }

    void clear() {
        m_renderer.clear();
    }

    template<class VertT, class DSS>
    requires(std::is_same_v<typename DSS::Vertex, VertT>) void draw_vertices(VertexArray<VertT> const& vbo, DrawState<DSS> const& draw_state) {
        m_renderer.draw_vertices(vbo, draw_state);
    }

    void resize(Util::Vector2u size) {
        m_fbo.resize(size);
    }

    void set_label(std::string const& label) {
        m_fbo.set_label(label);
    }

    auto const& color_texture() const { return m_fbo.color_texture(); }

    Renderer& renderer() { return m_renderer; }

private:
    llgl::opengl::FBO m_fbo;
    Renderer m_renderer;
};

}
