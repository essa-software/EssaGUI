#include <EssaGUI/Application.hpp>

#include "WidgetTreeRoot.hpp"
#include <EssaGUI/Graphics/ClipViewScope.hpp>
#include <EssaGUI/Graphics/ResourceManager.hpp>
#include <EssaGUI/Graphics/Window.hpp>
#include <EssaGUI/Overlays/ContextMenu.hpp>
#include <EssaGUI/Overlays/ToolWindow.hpp>
#include <EssaGUI/Widgets/Widget.hpp>

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
        m_cached_theme = Theme {};
        m_cached_theme->load_ini(m_resource_manager.require_lookup_resource(Gfx::ResourceId::asset("Theme.ini"), "")).release_value();
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
    for (auto& host_window : m_host_windows) {
        host_window.update();
    }
    redraw_all_host_windows();
}

}
