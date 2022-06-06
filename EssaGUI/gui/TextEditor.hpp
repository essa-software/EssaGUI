#pragma once

#include "EssaGUI/gfx/SFMLWindow.hpp"
#include "NotifyUser.hpp"
#include "ScrollableWidget.hpp"

#include <functional>

namespace GUI {

struct TextPosition {
    size_t line {};
    size_t column {};

    bool operator==(TextPosition const& other) const {
        return line == other.line && column == other.column;
    }

    bool operator<(TextPosition const& other) const {
        return line < other.line || (line == other.line && column < other.column);
    }

    bool operator>(TextPosition const& other) const {
        return !(*this < other) && !(*this == other);
    }
};

class TextEditor : public ScrollableWidget {
public:
    TextEditor(Container& parent)
        : ScrollableWidget(parent) { }

    virtual void handle_event(Event&) override;
    virtual void draw(GUI::SFMLWindow& window) const override;

    sf::String get_content() const;
    void set_content(sf::String content, NotifyUser = NotifyUser::Yes);
    void set_placeholder(std::string placeholder) { m_placeholder = placeholder; }

    enum class SetCursorSelectionBehavior {
        Extend,
        Clear,
        DontTouch
    };

    sf::String selected_text() const;

    std::function<void(sf::String)> on_change;
    std::function<void(sf::String)> on_enter;

private:
    TextPosition m_character_pos_from_mouse(Event& event);
    sf::Vector2f calculate_cursor_position() const;
    void erase_selected_text();
    virtual bool accepts_focus() const override { return true; }
    TextDrawOptions get_text_options() const;

    float line_height() const;
    virtual float content_height() const override;

    sf::Clock m_cursor_clock;

    enum class CursorDirection {
        Left,
        Right
    };
    void move_cursor(CursorDirection);
    void move_cursor_by_word(CursorDirection);

    // Set text cursor with updating scroll and selection if shift is pressed.
    void update_selection_after_set_cursor(SetCursorSelectionBehavior = SetCursorSelectionBehavior::Extend);

    bool m_shift_pressed = false;
    std::vector<sf::String> m_lines;
    sf::String m_placeholder;
    bool m_dragging = false;
    TextPosition m_cursor;
    TextPosition m_selection_start;
};

}