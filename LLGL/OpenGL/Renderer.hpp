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
    Renderer(unsigned fbo)
        : m_fbo(fbo) {
        { }
    }

    void clear(Util::Colorf const& color = Util::Colors::Black) {
        bind_if_not_bound(m_fbo);
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    template<class VertT, class DSS>
    requires(IsSameVertexLayout<typename DSS::Vertex, VertT>) void draw_vertices(VertexArray<VertT> const& vbo, DrawState<DSS> const& draw_state) {
        bind_if_not_bound(m_fbo);
        draw_state.apply();
        vbo.draw(draw_state.primitive_type());
    }

    template<class VertT, class DSS>
    requires(IsSameVertexLayout<typename DSS::Vertex, VertT>) void draw_vertices(VertexArray<VertT> const& vbo, DrawState<DSS> const& draw_state, size_t first, size_t size) {
        bind_if_not_bound(m_fbo);
        draw_state.apply();
        vbo.draw(draw_state.primitive_type(), first, size);
    }

private:
    static void bind_if_not_bound(unsigned fbo);

    unsigned m_fbo = 0;
};

}
