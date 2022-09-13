#pragma once

#include <EssaGUI/gfx/ResourceManager.hpp>
#include <EssaGUI/gui/HostWindow.hpp>
#include <LLGL/Resources/TTFFont.hpp>

namespace GUI {

class Application : public EventLoop {
public:
    Application();

    static Application& the();

    Gfx::ResourceManager const& resource_manager() const { return m_resource_manager; }

    llgl::TTFFont& font() const { return m_resource_manager.font(); }
    llgl::TTFFont& bold_font() const { return m_resource_manager.bold_font(); }
    llgl::TTFFont& fixed_width_font() const { return m_resource_manager.fixed_width_font(); }
    Theme const& theme() const;

    HostWindow& create_host_window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& = {});
    std::list<HostWindow>& host_windows() { return m_host_windows; }
    void redraw_all_host_windows();

private:
    virtual void tick() override;

    Gfx::ResourceManager m_resource_manager;
    mutable std::optional<Theme> m_cached_theme;
    std::list<HostWindow> m_host_windows;
};

}
