#pragma once

#include "Renderable.hpp"
#include "Renderer.hpp"

#include <LLGL/OpenGL/VAO.hpp>

#include <vector>

namespace llgl
{

// Stores vertexes into an internal array. Doesn't actually render anything
class BatchRenderer : public Renderer
    , public Renderable
{
public:
    virtual void apply_view(View const&) override { }
    virtual void render(Renderer&, DrawState) const override;
    virtual View view() const override { return {}; } // TODO
    virtual void draw_vao(opengl::VAO const&, opengl::PrimitiveType, DrawState const&) override {} // TODO until VAOs can be copied
    void draw_vao(opengl::VAO&&, opengl::PrimitiveType, DrawState const&);

private:
    opengl::PrimitiveType m_current_primitive_type {};
    DrawState m_current_state;
    std::vector<Vertex> m_vertexes;

    struct Command
    {
        opengl::PrimitiveType primitive_type {};
        DrawState state;
        opengl::VAO vao;
    };
    std::vector<Command> m_commands;
};

}
