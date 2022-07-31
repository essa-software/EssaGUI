#pragma once

#include "LLGL/OpenGL/Vertex.hpp"
#include "Renderer.hpp"

#include <vector>

namespace llgl {

// Uses programmable pipeline, used for OpenGL 3.2+
class CoreRenderer : public Renderer {
public:
    explicit CoreRenderer(Window& wnd)
        : m_window(wnd)
    {
    }

    virtual void apply_view(View const&) override;
    virtual View view() const override;
    virtual void draw_vao(opengl::VAO const&, opengl::PrimitiveType, DrawState const&) override;

private:
    View m_view;
    Window& m_window;
};

}
