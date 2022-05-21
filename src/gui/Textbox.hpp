#pragma once

#include "NotifyUser.hpp"
#include "Widget.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <limits>
#include <string>

namespace GUI {

class Textbox : public Widget {
    sf::String m_content, m_placeholder;
    sf::Color m_bg_color = sf::Color(220, 220, 220);
    sf::Color m_fg_color = sf::Color(120, 120, 120);
    sf::Color m_text_color = sf::Color(30, 30, 30);
    sf::Color m_placeholder_color = sf::Color(80, 80, 80);
    unsigned m_limit = 1024;
    bool m_has_decimal = false;
    bool m_dragging = false;
    bool m_has_limit = false;
    unsigned m_cursor = 0;
    unsigned m_selection_start = 0;
    double m_min_value = std::numeric_limits<double>::lowest();
    double m_max_value = std::numeric_limits<double>::max();

public:
    enum Type {
        TEXT,
        NUMBER
    };

    Type m_type = NUMBER;

    explicit Textbox(Container& c)
        : Widget(c) { }

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;
    void set_limit(unsigned limit) { m_limit = limit; }

    sf::String get_content() const { return (m_type == Type::NUMBER && m_content.isEmpty()) ? "0" : m_content; }
    void set_content(sf::String content, NotifyUser = NotifyUser::Yes);
    void set_data_type(Type type) { m_type = type; }
    void set_placeholder(std::string placeholder) { m_placeholder = placeholder; }
    void set_min_max_values(double min_value, double max_value) {
        m_min_value = min_value;
        m_max_value = max_value;
        m_has_limit = true;
    }

    enum class SetCursorSelectionBehavior {
        Extend,
        Clear,
        DontTouch
    };

    // Set text cursor with updating scroll and selection if shift is pressed.
    void interactive_set_cursor(unsigned, SetCursorSelectionBehavior = SetCursorSelectionBehavior::Extend);

    sf::String selected_text() const;

    std::function<void(std::string)> on_change;
    std::function<void(std::string)> on_enter;

private:
    bool can_insert_character(uint32_t) const;
    unsigned m_character_pos_from_mouse(Event& event);
    sf::Vector2f calculate_cursor_position() const;
    sf::Text generate_sf_text() const;
    void m_fit_in_range();
    bool find_decimal()const;
    std::string m_fix_content(std::string content) const;
    void erase_selected_text();
    virtual bool accepts_focus() const override { return true; }
    sf::Clock m_cursor_clock;

    enum class CursorDirection {
        Left,
        Right
    };
    void move_cursor_by_word(CursorDirection);

    float m_scroll = 0;
    bool m_shift_pressed = false;
};

}
