#pragma once

#include "Container.hpp"
#include "ToolWindow.hpp"
#include "Tooltip.hpp"
#include "WidgetTreeRoot.hpp"

#include <SFML/Graphics/View.hpp>
#include <iostream>
#include <list>

namespace GUI {

class Application : public WidgetTreeRoot {
public:
    explicit Application(sf::RenderWindow&);

    static Application& the();

    // TODO: Find a way for this to be private
    virtual void draw() override;

    virtual void handle_event(sf::Event) override;

    // TODO: Move it to some specialized class
    sf::Font font;
    sf::Font bold_font;
    sf::Font fixed_width_font;

    enum class NotificationLevel {
        Error
    };
    void spawn_notification(std::string message, NotificationLevel);

    template<class T = Overlay, class... Args>
    requires(std::is_base_of_v<Overlay, T>)
        T& open_overlay(Args&&... args) {
        return static_cast<T&>(open_overlay_impl(std::make_unique<T>(window(), std::forward<Args>(args)...)));
    }

    struct OpenOrFocusResult {
        ToolWindow* window {};
        bool opened {};
    };
    // FIXME: Generalize it like normal open_overlay
    OpenOrFocusResult open_or_focus_tool_window(sf::String title, std::string id);
    Overlay* focused_overlay() const { return m_focused_overlay; }

    template<class Callback>
    void for_each_overlay(Callback&& callback) {
        for (auto& wnd : m_overlays)
            callback(*wnd);
    }

    TooltipOverlay& add_tooltip(Tooltip t);
    void remove_closed_overlays();

    virtual sf::Vector2f position() const override { return {}; }
    virtual sf::Vector2f size() const override { return sf::Vector2f { window().getSize() }; }

private:
    virtual void handle_events() override;
    virtual void update() override;

    struct Notification {
        int remaining_ticks = 120;
        std::string message;
        NotificationLevel level {};
    };

    void draw_notification(Notification const&, float y) const;
    sf::Event transform_event(sf::Vector2f offset, sf::Event event) const;
    Overlay& open_overlay_impl(std::unique_ptr<Overlay>);

    using OverlayList = std::list<std::unique_ptr<Overlay>>;

    void focus_window(OverlayList::iterator);

    OverlayList m_overlays;
    sf::Vector2f m_next_overlay_position { 10, 10 + ToolWindow::TitleBarSize };
    Overlay* m_focused_overlay = nullptr;
    std::vector<Notification> m_notifications;
};

}
