#pragma once

#include <EssaGUI/gfx/ResourceManager.hpp>
#include <EssaGUI/gui/HostWindow.hpp>
#include <LLGL/Resources/TTFFont.hpp>

namespace GUI {

class Application {
public:
    explicit Application(GUI::Window&);

    static Application& the();

    Gfx::ResourceManager const& resource_manager() const { return m_resource_manager; }

    llgl::TTFFont& font() const { return m_resource_manager.font(); }
    llgl::TTFFont& bold_font() const { return m_resource_manager.bold_font(); }
    llgl::TTFFont& fixed_width_font() const { return m_resource_manager.fixed_width_font(); }
    Theme const& theme() const;

    HostWindow const& host_window() const { return m_host_window; }
    HostWindow& host_window() { return m_host_window; }

    void run();
    bool is_running() const { return host_window().is_running(); }

private:
    Gfx::ResourceManager m_resource_manager;
    mutable std::optional<Theme> m_cached_theme;
    HostWindow m_host_window;
};

}
