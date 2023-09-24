#include <Essa/GUI/Application.hpp>

#include <Essa/LLGL/Window/AbstractOpenGLHelper.hpp>
#include "WidgetTreeRoot.hpp"
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

#include <cassert>
#include <iostream>
#include <string>

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

Application::~Application() {
    s_the = nullptr;
}

Theme const& Application::theme() const {
    if (!m_cached_theme) {
        m_cached_theme = std::make_unique<Theme>();
        m_cached_theme->load_ini(m_resource_manager.require_lookup_resource({ Gfx::ResourceId::asset("Theme.ini"), "" })).release_value();
    }
    return *m_cached_theme;
}

HostWindow& Application::create_uninitialized_host_window() { return m_host_windows.emplace_back(); }

HostWindow& Application::create_host_window(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& settings) {
    return m_host_windows.emplace_back(size, title, settings);
}

void Application::remove_closed_host_windows() {
    std::erase_if(m_host_windows, [](auto const& window) { return window.is_closed(); });
}

void Application::redraw_all_host_windows() {
    for (auto& host_window : m_host_windows) {
        if (host_window.is_closed()) {
            continue;
        }
        host_window.do_draw();
    }
}

void Application::tick() {
    for (auto& host_window : m_host_windows) {
        host_window.handle_events();
    }
    if (on_tick) {
        on_tick();
    }
    remove_closed_host_windows();
    if (m_host_windows.empty()) {
        // I hadn't better place for this.
        // FIXME: Allow this to be disabled, and maybe more flexible (tool windows
        //        shouldn't inhibit close etc)
        quit();
    }
    for (auto& host_window : m_host_windows) {
        host_window.update();
    }
    redraw_all_host_windows();
}

}
