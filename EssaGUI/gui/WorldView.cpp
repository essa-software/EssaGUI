#include "WorldView.hpp"
#include <LLGL/OpenGL/Shaders/Basic330Core.hpp>

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
    , m_previous_projection(view.window().projection()) {

    if (current())
        return;

    auto projection = view.camera().projection();
    projection.set_viewport(Util::Recti { view.rect() });
    view.window().set_projection(view.camera().projection());
    view.window().set_view_matrix(view.camera().view_matrix());

    static llgl::opengl::shaders::Basic330Core shader;
    view.window().set_shader(&shader);

    // HACK: Normally (e.g when using EssaGUI's draw_vertices) EssaGUI
    // makes sure that the view is applied. It's not done when doing
    // direct LLGL rendering, so we need to do this manually here.
    view.window().renderer().apply_projection(projection);

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
    m_world_view.window().set_projection(m_previous_projection);
    m_world_view.window().set_view_matrix({});
}

}