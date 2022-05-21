#pragma once

#include "NotifyUser.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>
#include <functional>

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

    void set_active_without_action(bool state){m_active = state;}

    void set_toggleable(bool t) { m_toggleable = t; }

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    void set_active_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);

    std::function<void(bool)> on_change;

protected:
    sf::Color bg_color_for_state() const;
    sf::Color fg_color_for_state() const;
    sf::Color text_color_for_state() const;

    virtual bool accepts_focus() const override { return true; }

private:
    void click();

    sf::Color m_bg_color { 92, 89, 89 };
    sf::Color m_fg_color { 0, 0, 255 };
    sf::Color m_text_color { 255, 255, 255 };
    sf::Color m_active_bg_color { 0, 80, 255 };
    sf::Color m_active_fg_color { 0, 0, 255 };
    sf::Color m_active_text_color { 255, 255, 255 };

    bool m_toggleable { false };
    bool m_active { false };
    bool m_pressed_on_button { false };
};

}
