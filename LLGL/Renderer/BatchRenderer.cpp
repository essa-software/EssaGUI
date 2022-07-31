#include "BatchRenderer.hpp"

#include <EssaUtil/DelayedInit.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/Renderer/StateScope.hpp>

namespace llgl {

void BatchRenderer::render(Renderer& renderer, DrawState state) const
{
    for (auto const& command : m_commands) {
        auto this_state = command.state;
        this_state.view_matrix = state.view_matrix;
        renderer.draw_vao(command.vao, command.primitive_type, this_state);
    }
}

void BatchRenderer::draw_vao(opengl::VAO&& vao, opengl::PrimitiveType type, DrawState const& state)
{
    m_commands.push_back({ type, state, std::forward<opengl::VAO>(vao) });
}

}
