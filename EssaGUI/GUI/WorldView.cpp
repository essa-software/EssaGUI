#include "WorldView.hpp"

#include <EssaGUI/GUI/Application.hpp>
#include <LLGL/OpenGL/Transform.hpp>

namespace GUI {

WorldDrawScope const* WorldDrawScope::s_current = nullptr;

void WorldDrawScope::verify() {
    assert(current());
}

WorldDrawScope const* WorldDrawScope::current() {
    return s_current;
}

WorldDrawScope::WorldDrawScope(Gfx::Painter const& painter, ClearDepth clear_depth)
    : m_previous_projection(painter.builder().projection()) {

    if (current())
        return;

    glEnable(GL_DEPTH_TEST);

    if (clear_depth == ClearDepth::Yes)
        glClear(GL_DEPTH_BUFFER_BIT);

    llgl::set_viewport(m_previous_projection.viewport());

    m_parent = current();
    s_current = this;
}

WorldDrawScope::~WorldDrawScope() {
    s_current = m_parent;
    if (s_current)
        return;

    glDisable(GL_DEPTH_TEST);
}

}
