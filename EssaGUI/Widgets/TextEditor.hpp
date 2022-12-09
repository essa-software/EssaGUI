#pragma once

#include <EssaGUI/Graphics/Window.hpp>
#include <EssaGUI/NotifyUser.hpp>
#include <EssaGUI/TextEditing/SyntaxHighlighter.hpp>
#include <EssaGUI/TextEditing/TextPosition.hpp>
#include <EssaGUI/Widgets/ScrollableWidget.hpp>

#include <functional>

namespace GUI {

class TextEditor : public ScrollableWidget {
public:
    TextEditor();

    virtual void handle_event(Event&) override;
    virtual void draw(Gfx::Painter& window) const override;

    size_t line_count() const { return m_lines.size(); }
    Util::UString const& line(size_t idx) const { return m_lines[idx]; }
    Util::UString const& first_line() const { return m_lines.front(); }
    Util::UString const& last_line() const { return m_lines.back(); }

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

    void set_syntax_highlighter(std::unique_ptr<SyntaxHighlighter> h) { m_syntax_highlighter = std::move(h); }

protected:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

private:
    TextPosition m_character_pos_from_mouse(Event& event);
    Util::Vector2f calculate_cursor_position() const;
    void erase_selected_text();
    virtual void update() override;
    virtual bool accepts_focus() const override { return true; }
    virtual bool steals_focus() const override { return m_multiline; }
    virtual bool can_insert_codepoint(uint32_t) const { return true; }
    virtual void on_content_change() { }
    void did_content_change();

    float line_height() const;
    float left_margin() const;
    float character_width() const;
    virtual Util::Vector2f content_size() const override;
    virtual LengthVector initial_size() const override;
    virtual Util::Rectf scrollable_rect() const override;

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

    void regenerate_styles();

    std::vector<Util::UString> m_lines;
    std::vector<std::optional<size_t>> m_styles_for_letter;
    std::vector<TextStyle> m_styles;

    bool m_shift_pressed = false;
    bool m_dragging = false;
    TextPosition m_cursor;
    TextPosition m_selection_start;
    std::unique_ptr<SyntaxHighlighter> m_syntax_highlighter;
    bool m_content_changed = false;
};

}
