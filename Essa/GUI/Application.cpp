#include <Essa/GUI/Application.hpp>

#include "WidgetTreeRoot.hpp"
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Graphics/Window.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

#include <cassert>
#include <iostream>

namespace GUI {

Application* s_the {};

Application& Application::the() {
    assert(s_the);
    return *s_the;
}

Application::Application() {
    assert(!s_the);
    s_the = this;
}

Theme const& Application::theme() const {
    if (!m_cached_theme) {
        m_cached_theme = std::make_unique<Theme>();
        m_cached_theme->load_ini(m_resource_manager.require_lookup_resource({ Gfx::ResourceId::asset("Theme.ini"), "" })).release_value();
    }
    return *m_cached_theme;
}

HostWindow& Application::create_host_window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& settings) {
    return m_host_windows.emplace_back(size, title, settings);
}

void Application::redraw_all_host_windows() {
    for (auto& host_window : m_host_windows) {
        host_window.do_draw();
    }
}

void Application::tick() {
    for (auto& host_window : m_host_windows) {
        host_window.handle_events();
    }
    if (on_update) {
        on_update();
    }
    for (auto& host_window : m_host_windows) {
        host_window.update();
    }
    redraw_all_host_windows();
}

}
