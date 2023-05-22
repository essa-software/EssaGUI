#pragma once

#include <Essa/LLGL/OpenGL/DrawState.hpp>
#include <Essa/LLGL/OpenGL/FBO.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>

namespace llgl {

class Framebuffer {
public:
    explicit Framebuffer(Util::Size2u size)
        : m_fbo(size)
        , m_renderer(m_fbo.id()) {
        m_renderer.m_size = size;
    }

    void clear() { m_renderer.clear(); }

    template<class VertT, class DSS>
        requires(std::is_same_v<typename DSS::Vertex, VertT>)
    void draw_vertices(VertexArray<VertT> const& vbo, DrawState<DSS> const& draw_state) {
        m_renderer.draw_vertices(vbo, draw_state);
    }

    void resize(Util::Size2u size) {
        m_fbo.resize(size);
        m_renderer.m_size = size;
    }

    void set_label(std::string const& label) { m_fbo.set_label(label); }

    auto const& color_texture() const { return m_fbo.color_texture(); }

    Renderer& renderer() { return m_renderer; }
    auto const& fbo() const { return m_fbo; }

private:
    llgl::opengl::FBO m_fbo;
    Renderer m_renderer;
};

}
