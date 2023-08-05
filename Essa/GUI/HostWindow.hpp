#pragma once

#include <Essa/GUI/Graphics/GUIBuilder.hpp>
#include <Essa/GUI/Overlay.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Overlays/Tooltip.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/MDI/Host.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/DelayedInit.hpp>

namespace GUI {

// The WTR that can "host" Overlays (ToolWindows etc.). It corresponds
// to the operating system's window.
class HostWindow
    : public llgl::Window
    , public WidgetTreeRoot {
public:
    friend Gfx::Painter;
    friend Gfx::GUIBuilder;
    HostWindow() = default;
    [[deprecated]] HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& = {});

    virtual void setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const&) override;
    virtual void close() override;
    virtual void set_size(Util::Size2i size) override { llgl::Window::set_size(size.cast<unsigned>()); }

    // TODO: Find a way for this to be private
    void do_draw();
    void handle_events(); // Called by Application

    // NOTE: The opened context menu is modal, meaning that this
    //       function won't return until user chooses an action
    //       or dismisses the menu.
    void open_context_menu(ContextMenu, Util::Point2i position);
    TooltipOverlay& add_tooltip(Tooltip t);

    virtual Util::Point2i position() const override { return llgl::Window::position(); }
    virtual Util::Size2i size() const override { return llgl::Window::size().cast<int>(); }
    Util::Recti rect() const { return { {}, size() }; }

    virtual void center_on_screen() override;

    void set_background_color(Util::Color color) { m_background_color = color; }

    // Override default event handler. If this returns Accepted,
    // no events will be passed to widgets.
    std::function<GUI::Widget::EventHandlerResult(GUI::Event const&)> on_event;

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

    // FIXME: Rename this to set_main_widget after removing the old set_main_widget
    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_root_widget(Args&&... args) {
        m_legacy_mdi_host = nullptr;
        return WidgetTreeRoot::set_main_widget<T>(std::forward<Args>(args)...);
    }
    // FIXME: Rename this to main_widget after removing the old main_widget
    Widget* root_widget() { return WidgetTreeRoot::main_widget(); }

    void show_modal();
    bool is_modal() const { return m_modal; }

    // --- Deprecated compatibility things start here ---
    // Get rid of them when everyone is updated to explicit MDI
    auto legacy_mdi_host() { return m_legacy_mdi_host; }
    template<class T, class... Args> [[deprecated]] T& set_main_widget(Args&&... args) {
        assert(m_legacy_mdi_host);
        return m_legacy_mdi_host->set_background_widget<T>(std::forward<Args>(args)...);
    }
    [[deprecated]] Widget const* main_widget() const {
        assert(m_legacy_mdi_host);
        return m_legacy_mdi_host->background_widget();
    }
    [[deprecated]] Widget* main_widget() {
        assert(m_legacy_mdi_host);
        return m_legacy_mdi_host->background_widget();
    }
    template<class T, class... Args> [[deprecated]] decltype(auto) open_overlay(Args&&... a) {
        assert(m_legacy_mdi_host);
        return m_legacy_mdi_host->open_overlay<T>(std::forward<Args>(a)...);
    }
    void remove_closed_overlays() {
        if (m_legacy_mdi_host) {
            m_legacy_mdi_host->remove_closed_overlays();
        }
    }
    // --- END ---

private:
    friend WidgetTreeRoot;
    friend class Application;

    Util::Color m_background_color;
    Util::DelayedInit<Gfx::Painter> m_painter;

    MDI::Host* m_legacy_mdi_host = nullptr;
    bool m_modal = false;
};

}
