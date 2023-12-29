#include <Essa/GUI/Application.hpp>

#include "WidgetTreeRoot.hpp"
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/Window/AbstractOpenGLHelper.hpp>

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

Application::~Application() { s_the = nullptr; }

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

WindowRoot& Application::setup_window_root(HostWindow& window, std::unique_ptr<WindowRoot> root) {
    // We expect that root will initialize/open the window.
    assert(!window.is_closed());
    auto* root_ptr = root.get();
    window.set_root(std::move(root));
    return *root_ptr;
}

std::vector<DevToolsObject const*> Application::dev_tools_children() const {
    std::vector<DevToolsObject const*> vec;
    for (auto const& window : m_host_windows) {
        vec.push_back(&window);
    }
    return vec;
}

namespace Detail {

SimpleApplicationBase::SimpleApplicationBase(Util::UString const& title, Util::Size2u window_size, std::unique_ptr<Widget> main_widget)
    : m_window(create_host_window(window_size, title)) {
    if (window_size == Util::Size2u {}) {
        m_window.set_size({ 500, 500 });
        m_window.maximize();
    }
    m_window.center_on_screen();
    m_window.set_created_main_widget(std::move(main_widget));
}

Widget& SimpleApplicationBase::main_widget_impl() {
    assert(m_window.main_widget());
    return *m_window.main_widget();
}

}

}
