#pragma once

#include <Essa/GUI/Overlay.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Overlays/Tooltip.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/LLGL/Window/Window.hpp>

namespace GUI {

// The WTR that can "host" Overlays (ToolWindows etc.). It corresponds
// to the operating system's window.
class HostWindow
    : public WidgetTreeRoot
    , public llgl::Window {
public:
    explicit HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& = {});

    // TODO: Find a way for this to be private
    void do_draw();
    virtual void handle_events() override; // Called by Application
    virtual void update() override;        // Called by Application

    virtual void handle_event(GUI::Event const&) override;

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

    virtual Util::Point2i position() const override { return {}; }
    virtual Util::Size2i size() const override { return llgl::Window::size().cast<int>(); }

    void focus_overlay(Overlay&);

    void set_background_color(Util::Color color) { m_background_color = color; }

    using WidgetTreeRoot::rect;

private:
    Overlay& open_overlay_impl(std::unique_ptr<Overlay>);

    using OverlayList = std::list<std::unique_ptr<Overlay>>;

    void focus_window(OverlayList::iterator);

    OverlayList m_overlays;
    Util::Point2f m_next_overlay_position { 10, 10 + theme().tool_window_title_bar_size };
    Overlay* m_focused_overlay = nullptr;
    Util::Color m_background_color;
    Gfx::Painter m_painter { renderer() };
};

}
