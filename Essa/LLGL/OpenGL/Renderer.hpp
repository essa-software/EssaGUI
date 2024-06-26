#pragma once

#include "DrawState.hpp"
#include "OpenGL.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"

namespace llgl {

class Renderer {
public:
    Util::Size2u size() const {
        return m_size;
    }

    void clear(Util::Colorf const& color = Util::Colors::Black) {
        bind_if_not_bound(m_fbo);
        OpenGL::ClearColor(color.r, color.g, color.b, color.a);
        OpenGL::Clear(GL_COLOR_BUFFER_BIT);
    }

    template<class VertT, class DSS>
        requires(IsSameVertexLayout<typename DSS::Vertex, VertT>)
    void draw_vertices(VertexArray<VertT> const& vbo, DrawState<DSS> const& draw_state) {
        bind_if_not_bound(m_fbo);
        draw_state.apply();
        vbo.draw(draw_state.primitive_type());
    }

    template<class VertT, class DSS>
        requires(IsSameVertexLayout<typename DSS::Vertex, VertT>)
    void draw_vertices(VertexArray<VertT> const& vbo, DrawState<DSS> const& draw_state, size_t first, size_t size) {
        bind_if_not_bound(m_fbo);
        draw_state.apply();
        vbo.draw(draw_state.primitive_type(), first, size);
    }

private:
    explicit Renderer(unsigned fbo)
        : m_fbo(fbo) {
        { }
    }

    friend class Window;
    friend class Framebuffer;

    static void bind_if_not_bound(unsigned fbo);

    unsigned m_fbo = 0;
    Util::Size2u m_size;
};

}
