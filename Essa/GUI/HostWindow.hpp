#pragma once

#include "Essa/GUI/Graphics/GUIBuilder.hpp"
#include <Essa/GUI/Overlay.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Overlays/Tooltip.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/LLGL/Window/Window.hpp>

namespace GUI {

// The WTR that can "host" Overlays (ToolWindows etc.). It corresponds
// to the operating system's window.
class HostWindow : public llgl::Window {
public:
    friend Gfx::Painter;
    friend Gfx::GUIBuilder;
    explicit HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& = {});

    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_main_widget(Args&&... args) {
        return m_fullscreen_overlay->set_main_widget<T>(std::forward<Args>(args)...);
    }

    template<class T> auto& set_created_main_widget(std::shared_ptr<T> w) {
        return m_fullscreen_overlay->set_created_main_widget<T>(std::move(w));
    }

    auto* main_widget() { return m_fullscreen_overlay->main_widget(); }

    // TODO: Find a way for this to be private
    void do_draw();
    void handle_events(); // Called by Application
    void update();        // Called by Application

    void handle_event(GUI::Event const&);

    template<class T = Overlay, class... Args>
        requires(std::is_base_of_v<Overlay, T>)
    T& open_overlay(Args&&... args) {
        return static_cast<T&>(open_overlay_impl(std::make_unique<T>(*this, std::forward<Args>(args)...)));
    }

    struct OpenOrFocusResult {
        ToolWindow* window {};
        bool opened {};
    };
    // FIXME: Generalize it like normal open_overlay
    OpenOrFocusResult open_or_focus_tool_window(Util::UString title, std::string id);

    // NOTE: The opened context menu is modal, meaning that this
    //       function won't return until user chooses an action
    //       or dismisses the menu.
    void open_context_menu(ContextMenu, Util::Point2i position);

    Overlay* focused_overlay() const { return m_focused_overlay; }

    template<class Callback> void for_each_overlay(Callback&& callback) {
        for (auto& wnd : m_overlays)
            callback(*wnd);
    }

    TooltipOverlay& add_tooltip(Tooltip t);
    void remove_closed_overlays();

    Util::Size2i size() const { return llgl::Window::size().cast<int>(); }
    Util::Recti rect() const { return { {}, size() }; }

    void focus_overlay(Overlay&);

    void set_background_color(Util::Color color) { m_background_color = color; }

    // Override default event handler. If this returns Accepted,
    // no events will be passed to widgets.
    std::function<GUI::Widget::EventHandlerResult(GUI::Event const&)> on_event;

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

private:
    Overlay& open_overlay_impl(std::unique_ptr<Overlay>);

    using OverlayList = std::list<std::unique_ptr<Overlay>>;

    void focus_window(OverlayList::iterator);

    Overlay* m_fullscreen_overlay = nullptr;

    OverlayList m_overlays;
    Util::Point2f m_next_overlay_position;
    Overlay* m_focused_overlay = nullptr;
    Overlay* m_hovered_overlay = nullptr;
    Util::Color m_background_color;
    Gfx::Painter m_painter { renderer() };
};

}
