#include "WorldView.hpp"
#include "EssaGUI/gui/Application.hpp"
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
    , m_previous_projection(GUI::Application::the().window().projection()) {

    if (current())
        return;

    auto& window = GUI::Application::the().window();

    auto projection = view.camera().projection();
    projection.set_viewport(Util::Recti { view.rect() });
    window.set_projection(view.camera().projection());
    window.set_view_matrix(view.camera().view_matrix());

    static llgl::opengl::shaders::Basic330Core shader;
    window.set_shader(&shader);

    // HACK: Normally (e.g when using EssaGUI's draw_vertices) EssaGUI
    // makes sure that the view is applied. It's not done when doing
    // direct LLGL rendering, so we need to do this manually here.
    window.renderer().apply_projection(projection);

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
    auto& window = GUI::Application::the().window();
    window.set_projection(m_previous_projection);
    window.set_view_matrix({});
}

}
