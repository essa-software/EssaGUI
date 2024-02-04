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
    virtual void set_size(Util::Size2i size) override { llgl::Window::set_size(size.cast<unsigned>()); }
    virtual HostWindow& host_window() override { return *this; }

    // TODO: Find a way for this to be private
    void do_draw();
    void handle_events(); // Called by Application

    // NOTE: The opened context menu is modal, meaning that this
    //       function won't return until user chooses an action
    //       or dismisses the menu.
    void open_context_menu(ContextMenu, Util::Point2i position);
    // Position is relative to this HostWindow.
    TooltipOverlay& add_tooltip(Util::Point2u position, Tooltip t) const;

    virtual Util::Point2i position() const override { return llgl::Window::position(); }
    virtual Util::Size2i size() const override { return llgl::Window::size().cast<int>(); }
    Util::Recti rect() const { return { {}, size() }; }

    virtual void center_on_screen() override;

    void set_background_color(Util::Color color) { m_background_color = color; }

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

    // FIXME: Rename this to set_main_widget after removing the old set_main_widget
    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_root_widget(Args&&... args) {
        return WidgetTreeRoot::set_main_widget<T>(std::forward<Args>(args)...);
    }
    // FIXME: Rename this to main_widget after removing the old main_widget
    Widget* root_widget() { return WidgetTreeRoot::main_widget(); }

    void show_modal(HostWindow* parent);
    bool is_modal() const { return m_modal; }

    virtual Util::UString dev_tools_name() const override { return title(); }

    std::function<void()> on_close;

private:
    friend WidgetTreeRoot;
    friend class Application;

    Util::Color m_background_color;
    Util::DelayedInit<Gfx::Painter> m_painter;

    bool m_modal = false;
    bool m_is_blocked_by_modal_dialog = false;
};

}
