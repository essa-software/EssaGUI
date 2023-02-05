#pragma once

#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>

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

    HostWindow& create_host_window(Util::Vector2i size, Util::UString const& title, llgl::WindowSettings const& = {});
    std::list<HostWindow>& host_windows() { return m_host_windows; }
    void redraw_all_host_windows();

    // This is called every tick, just before updating host windows.
    std::function<void()> on_tick;

private:
    virtual void tick() override;

    Gfx::ResourceManager m_resource_manager;
    mutable std::unique_ptr<Theme> m_cached_theme;
    std::list<HostWindow> m_host_windows;
};

// An application with a single maximized by default window. This assumes
// that main widget doesn't take any arguments.
template<class W>
requires(std::is_base_of_v<Widget, W>) class SimpleApplication : public Application {
public:
    template<class... Args>
    requires std::is_constructible_v<W, Args...>
    SimpleApplication(Util::UString const& title, Util::Vector2i window_size = {}, Args&&... args)
        : m_window(create_host_window(window_size, title)) {
        if (window_size == Util::Vector2i {}) {
            m_window.set_size({ 500, 500 });
            m_window.maximize();
        }
        m_window.center_on_screen();
        m_window.set_main_widget<W>(std::forward<Args>(args)...);
    }

    auto& window() { return m_window; }
    W& main_widget() {
        assert(m_window.main_widget());
        return static_cast<W&>(*m_window.main_widget());
    }

private:
    void setup();

    GUI::HostWindow& m_window;
};

}
