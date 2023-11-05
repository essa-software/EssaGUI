#include "WorldView.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>

namespace GUI {

WorldDrawScope const* WorldDrawScope::s_current = nullptr;

void WorldDrawScope::verify() { assert(current()); }

WorldDrawScope const* WorldDrawScope::current() { return s_current; }

WorldDrawScope::WorldDrawScope(Gfx::Painter const& painter, ClearDepth clear_depth)
    : m_previous_projection(painter.projection()) {

    if (current())
        return;

    OpenGL::Enable(GL_DEPTH_TEST);

    if (clear_depth == ClearDepth::Yes)
        OpenGL::Clear(GL_DEPTH_BUFFER_BIT);

    llgl::set_viewport(m_previous_projection.viewport());

    m_parent = current();
    s_current = this;
}

WorldDrawScope::~WorldDrawScope() {
    s_current = m_parent;
    if (s_current)
        return;

    OpenGL::Disable(GL_DEPTH_TEST);
}

}
