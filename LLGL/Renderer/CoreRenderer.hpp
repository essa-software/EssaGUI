#pragma once

#include "LLGL/OpenGL/Vertex.hpp"
#include "Renderer.hpp"

#include <vector>

namespace llgl {

// Uses programmable pipeline, used for OpenGL 3.2+
class CoreRenderer : public Renderer {
public:
    explicit CoreRenderer(Window& wnd)
        : m_window(wnd) {
    }

    virtual void apply_projection(Projection const&) override;
    virtual Projection projection() const override;
    virtual void draw_vao(opengl::VAO const&, opengl::PrimitiveType, DrawState const&) override;

private:
    Projection m_projection;
    Window& m_window;
};

}
