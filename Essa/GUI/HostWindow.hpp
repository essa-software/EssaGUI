#pragma once

#include <Essa/GUI/Overlay.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Overlays/Tooltip.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>

namespace GUI {

// The WTR that can "host" Overlays (ToolWindows etc.). It corresponds
// to the operating system's window.
class HostWindow : public WidgetTreeRoot {
public:
    explicit HostWindow(Util::Vector2i size, Util::UString const& title, llgl::WindowSettings const& = {});

    // TODO: Find a way for this to be private
    void do_draw();
    virtual void handle_events() override; // Called by Application
    virtual void update() override;        // Called by Application

    virtual void handle_event(GUI::Event const&) override;

    GUI::Window& window() { return m_window; }
    GUI::Window const& window() const { return m_window; }

    enum class NotificationLevel {
        Error
    };
    void spawn_notification(Util::UString message, NotificationLevel);

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
    void open_context_menu(ContextMenu, Util::Vector2f position);

    Overlay* focused_overlay() const { return m_focused_overlay; }

    template<class Callback>
    void for_each_overlay(Callback&& callback) {
        for (auto& wnd : m_overlays)
            callback(*wnd);
    }

    TooltipOverlay& add_tooltip(Tooltip t);
    void remove_closed_overlays();

    virtual Util::Vector2f position() const override { return {}; }
    virtual Util::Vector2f size() const override { return Util::Vector2f { window().size() }; }

    void focus_overlay(Overlay&);

private:
    struct Notification {
        int remaining_ticks = 120;
        Util::UString message;
        NotificationLevel level {};
    };

    void draw_notification(Notification const&, float y);
    Overlay& open_overlay_impl(std::unique_ptr<Overlay>);

    using OverlayList = std::list<std::unique_ptr<Overlay>>;

    void focus_window(OverlayList::iterator);

    GUI::Window m_window;
    OverlayList m_overlays;
    Util::Vector2f m_next_overlay_position { 10, 10 + theme().tool_window_title_bar_size };
    Overlay* m_focused_overlay = nullptr;
    std::vector<Notification> m_notifications;
    Gfx::Painter m_painter { window().renderer() };
};

}
