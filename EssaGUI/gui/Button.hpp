#pragma once

#include "NotifyUser.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <optional>

namespace GUI {

class Button : public Widget {
public:
    explicit Button(Container& c)
        : Widget(c) { }

    std::function<void()> on_click;

    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow& window) const override = 0;

    bool is_active() const { return m_active; }

    void set_active(bool active, NotifyUser notify_user = NotifyUser::Yes) {
        if (m_active != active) {
            m_active = active;
            if (notify_user == NotifyUser::Yes && on_change)
                on_change(active);
        }
    }

    void set_active_without_action(bool state) { m_active = state; }
    void set_toggleable(bool t) { m_toggleable = t; }

    std::function<void(bool)> on_change;

    // TODO: Replace this with full theme override system
    virtual Theme::ButtonColors& override_button_colors() {
        if (m_button_colors_override)
            return *m_button_colors_override;
        m_button_colors_override = default_button_colors();
        return *m_button_colors_override;
    }

protected:
    sf::Color color_for_state(sf::Color) const;
    sf::Color bg_color_for_state() const;
    sf::Color text_color_for_state() const;

    virtual Theme::ButtonColors default_button_colors() const = 0;

    virtual bool accepts_focus() const override { return true; }

private:
    void click();

    std::optional<Theme::ButtonColors> m_button_colors_override;

    bool m_toggleable { false };
    bool m_active { false };
    bool m_pressed_on_button { false };
};

}
