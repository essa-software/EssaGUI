#include "WorldView.hpp"

#include <EssaGUI/gui/Application.hpp>
#include <LLGL/OpenGL/Transform.hpp>

namespace GUI {

WorldDrawScope const* WorldDrawScope::s_current = nullptr;

void WorldDrawScope::verify() {
    assert(current());
}

WorldDrawScope const* WorldDrawScope::current() {
    return s_current;
}

WorldDrawScope::WorldDrawScope(WorldView const& view, ClearDepth clear_depth)
    : m_world_view(view)
    , m_previous_projection(view.host_window().window().projection()) {

    if (current())
        return;

    auto& window = view.host_window().window();
    window.set_active();

    glEnable(GL_DEPTH_TEST);

    if (clear_depth == ClearDepth::Yes)
        glClear(GL_DEPTH_BUFFER_BIT);

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
