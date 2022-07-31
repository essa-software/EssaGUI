#include "Renderer.hpp"

#include "LLGL/OpenGL/Utils.hpp"
#include "Renderable.hpp"

#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/Renderer/StateScope.hpp>

namespace llgl {

void Renderer::clear(std::optional<Util::Color> color)
{
    if (color)
        opengl::set_clear_color(*color);
    opengl::clear_enabled();
}

void Renderer::render_object(Renderable const& renderable, DrawState state)
{
    renderable.render(*this, state);
}

}
