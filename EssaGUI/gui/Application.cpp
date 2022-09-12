#include "Application.hpp"

#include "ContextMenu.hpp"
#include "EssaGUI/gfx/ResourceManager.hpp"
#include "ToolWindow.hpp"
#include "Widget.hpp"
#include "WidgetTreeRoot.hpp"
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <EssaGUI/gfx/Window.hpp>

#include <cassert>
#include <iostream>

namespace GUI {

Application* s_the {};

Application& Application::the() {
    assert(s_the);
    return *s_the;
}

Application::Application(GUI::Window& wnd)
    : m_host_window(wnd) {
    assert(!s_the);
    s_the = this;
}

Theme const& Application::theme() const {
    if (!m_cached_theme) {
        m_cached_theme = Theme {};
        m_cached_theme->load_ini(m_resource_manager.require_lookup_resource(Gfx::ResourceId::asset("Theme.ini"), "")).release_value();
    }
    return *m_cached_theme;
}

void Application::run() {
    m_host_window.run();
}

}
