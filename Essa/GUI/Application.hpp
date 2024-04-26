#pragma once

#include <Essa/GUI/EventLoop.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>
#include <Essa/LLGL/Window/WindowSettings.hpp>
#include <type_traits>

namespace GUI {

class HostWindow;
class Theme;
class Widget;
class WindowRoot;

class Application
    : public EventLoop
    , public DevToolsObject {
public:
    Application();
    ~Application();

    static Application& the();

    Gfx::ResourceManager const& resource_manager() const {
        return m_resource_manager;
    }
    Gfx::ResourceManager& resource_manager() {
        return m_resource_manager;
    }

    llgl::TTFFont& font() const {
        return m_resource_manager.font();
    }
    llgl::TTFFont& bold_font() const {
        return m_resource_manager.bold_font();
    }
    llgl::TTFFont& fixed_width_font() const {
        return m_resource_manager.fixed_width_font();
    }
    Theme const& theme() const;

    HostWindow& create_uninitialized_host_window();
    HostWindow& create_host_window(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& = {});

    template<class T>
        requires(std::is_base_of_v<WindowRoot, T>)
    struct OpenedHostWindow {
        HostWindow& window;
        T& root;
    };

    // TODO: Support passing window settings here (somehow)
    template<class T, class... Args>
        requires(std::is_base_of_v<WindowRoot, T>)
    OpenedHostWindow<T> open_host_window(Args&&... args) {
        auto& window = create_uninitialized_host_window();
        auto& root = setup_window_root(window, std::make_unique<T>(window, std::forward<Args>(args)...));

        return {
            .window = window,
            .root = static_cast<T&>(root),
        };
    }

    std::list<HostWindow>& host_windows() {
        return m_host_windows;
    }
    void remove_closed_host_windows();
    void redraw_all_host_windows();

    // This is called every tick, just before updating host windows.
    std::function<void()> on_tick;

    virtual std::vector<DevToolsObject const*> dev_tools_children() const override;
    virtual Util::UString dev_tools_name() const override {
        return "Application";
    }

private:
    virtual void tick() override;
    static WindowRoot& setup_window_root(HostWindow&, std::unique_ptr<WindowRoot>);

    Gfx::ResourceManager m_resource_manager;
    mutable std::unique_ptr<Theme> m_cached_theme;
    std::list<HostWindow> m_host_windows;
};

namespace Detail {

class SimpleApplicationBase : public Application {
public:
    auto& window() {
        return m_window;
    }

protected:
    explicit SimpleApplicationBase(Util::UString const& title, Util::Size2u window_size, std::unique_ptr<Widget> main_widget);

    Widget& main_widget_impl();

private:
    GUI::HostWindow& m_window;
};

}

// An application with a single maximized by default window.
template<class W>
    requires(std::is_base_of_v<Widget, W>)
class SimpleApplication : public Detail::SimpleApplicationBase {
public:
    template<class... Args>
        requires std::is_constructible_v<W, Args...>
    SimpleApplication(Util::UString const& title, Util::Size2u window_size = {}, Args&&... args)
        : Detail::SimpleApplicationBase(title, window_size, std::make_unique<W>(std::forward<Args>(args)...)) { }

    W& main_widget() {
        return static_cast<W&>(main_widget_impl());
    }
};

}
