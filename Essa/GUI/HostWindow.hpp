#pragma once

#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/Tooltip.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/DelayedInit.hpp>

namespace GUI {

// The WTR that can "host" Overlays (ToolWindows etc.). It corresponds
// to the operating system's window.
class HostWindow
    : public llgl::Window
    , public WidgetTreeRoot {
public:
    friend class Gfx::Painter;
    friend class Gfx::GUIBuilder;
    HostWindow() = default;
    [[deprecated]] HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& = {});

    virtual void setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const&) override;
    virtual void close() override;
    virtual void set_size(Util::Size2i size) override {
        llgl::Window::set_size(size.cast<unsigned>());
    }
    virtual HostWindow& host_window() override {
        return *this;
    }

    // TODO: Find a way for this to be private
    void do_draw();
    void handle_events(); // Called by Application

    // The opened context menu is modal, meaning that this
    // function won't return until user chooses an action
    // or dismisses the menu.
    // Position is relative to this HostWindow.
    [[deprecated("Use open_context_menu_non_blocking()")]] void open_context_menu(Util::Point2i position, ContextMenu);
    void open_context_menu_non_blocking(Util::Point2i position, ContextMenu);
    // Position is relative to this HostWindow.
    TooltipOverlay& add_tooltip(Util::Point2i position, Tooltip t) const;

    virtual Util::Point2i host_position() const override {
        return {};
    }
    virtual Util::Size2i size() const override {
        return llgl::Window::size().cast<int>();
    }
    Util::Recti rect() const {
        return { {}, size() };
    }

    virtual void center_on_screen() override;

    void set_background_color(Util::Color color) {
        m_background_color = color;
    }

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

    // FIXME: Rename this to set_main_widget after removing the old set_main_widget
    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_root_widget(Args&&... args) {
        return WidgetTreeRoot::set_main_widget<T>(std::forward<Args>(args)...);
    }
    // FIXME: Rename this to main_widget after removing the old main_widget
    Widget* root_widget() {
        return WidgetTreeRoot::main_widget();
    }

    void show_modal(HostWindow* parent);
    void show_modal_non_blocking(HostWindow& parent);
    bool is_modal() const {
        return m_modal_parent != nullptr;
    }

    virtual Util::UString dev_tools_name() const override {
        return title();
    }

    std::function<void()> on_close;

private:
    friend WidgetTreeRoot;
    friend class Application;

    Util::Color m_background_color;
    Util::DelayedInit<Gfx::Painter> m_painter;

    HostWindow* m_modal_parent = nullptr;
    bool m_is_blocked_by_modal_dialog = false;
};

// Closes the host window when destroyed. Also ensures that only one
// HostWindow is open at a time (closes the previous one on assignment).
class ScopedHostWindow {
public:
    ScopedHostWindow() = default;
    ~ScopedHostWindow() {
        if (m_window) {
            m_window->close();
        }
    }

    void set_window(HostWindow& window) {
        if (m_window) {
            m_window->close();
        }
        m_window = &window;
        window.on_close = [this] { m_window = nullptr; };
    }
    HostWindow* window() {
        return m_window;
    }
    HostWindow const* window() const {
        return m_window;
    }

private:
    HostWindow* m_window = nullptr;
};

}
