#pragma once

#include "NotifyUser.hpp"
#include "ScrollableWidget.hpp"
#include <EssaGUI/gfx/Window.hpp>

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
    virtual void handle_event(Event&) override;
    virtual void draw(Gfx::Painter& window) const override;

    Util::UString content() const;
    void set_content(Util::UString content, NotifyUser = NotifyUser::Yes);
    bool is_empty() const { return m_lines.empty() || (m_lines.size() == 1 && m_lines[0].is_empty()); }

    CREATE_VALUE(Util::UString, placeholder, "")
    CREATE_BOOLEAN(multiline, true)

    enum class SetCursorSelectionBehavior {
        Extend,
        Clear,
        DontTouch
    };

    Util::UString selected_text() const;

    std::function<void(Util::UString const&)> on_change;

    // The callback that is run when pressing Enter, or Ctrl + Enter
    // for multiline textboxes.
    std::function<void(Util::UString const&)> on_enter;

protected:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

private:
    TextPosition m_character_pos_from_mouse(Event& event);
    Util::Vector2f calculate_cursor_position() const;
    void erase_selected_text();
    virtual bool accepts_focus() const override { return true; }
    virtual bool steals_focus() const override { return m_multiline; }
    virtual bool can_insert_codepoint(uint32_t) const { return true; }
    virtual void on_content_change() { }

    float line_height() const;
    float left_margin() const;
    float character_width() const;
    virtual Util::Vector2f content_size() const override;
    virtual LengthVector initial_size() const override;

    TextPosition real_cursor_position() const;

    // sf::Clock m_cursor_clock;

    enum class CursorDirection {
        Left,
        Right
    };
    void move_cursor(CursorDirection);
    void move_cursor_by_word(CursorDirection);

    void insert_codepoint(uint32_t);

    // Set text cursor with updating scroll and selection if shift is pressed.
    void update_selection_after_set_cursor(SetCursorSelectionBehavior = SetCursorSelectionBehavior::Extend);

    bool m_shift_pressed = false;
    std::vector<Util::UString> m_lines;
    bool m_dragging = false;
    TextPosition m_cursor;
    TextPosition m_selection_start;
};

}
