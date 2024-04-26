#pragma once

#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/TextEditing/SyntaxHighlighter.hpp>
#include <Essa/GUI/TextEditing/TextPosition.hpp>
#include <Essa/GUI/Widgets/ScrollableWidget.hpp>

#include <functional>

namespace GUI {

class TextEditor : public ScrollableWidget {
public:
    TextEditor();

    virtual void draw(Gfx::Painter& window) const override;

    size_t line_count() const { return m_lines.size(); }
    Util::UString const& line(size_t idx) const { return m_lines[idx]; }
    Util::UString const& first_line() const { return m_lines.front(); }
    Util::UString const& last_line() const { return m_lines.back(); }

    Util::UString content() const;
    void set_content(Util::UString content, NotifyUser = NotifyUser::Yes);
    bool is_empty() const { return m_lines.empty() || (m_lines.size() == 1 && m_lines[0].is_empty()); }

    CREATE_VALUE(Util::UString, placeholder, "")

    void set_editable(bool e) { m_editable = e; }
    bool is_editable() const { return m_editable; }
    void set_multiline(bool m);
    bool is_multiline() const { return m_multiline; }

    enum class SetCursorSelectionBehavior { Extend, Clear, DontTouch };

    Util::UString selected_text() const;

    std::function<void(Util::UString const&)> on_change;

    // The callback that is run when pressing Enter, or Ctrl + Enter
    // for multiline textboxes.
    std::function<void(Util::UString const&)> on_enter;

    void set_syntax_highlighter(std::unique_ptr<SyntaxHighlighter> h) { m_syntax_highlighter = std::move(h); }

    struct ErrorSpan {
        enum class Type { Note, Warning, Error };
        Type type;
        TextRange range;
    };
    void set_error_spans(std::vector<ErrorSpan> errors) { m_error_spans = std::move(errors); }

    // Callback is void callback(size_t line, size_t start_column, size_t end_column)
    // Note: This is overflow-safe.
    template<class Callback> void for_each_line_in_range(TextRange range, Callback&& callback) const {
        for (size_t line = range.start.line; line <= range.end.line; line++) {
            auto start = line == range.start.line ? range.start.column : 0;
            auto end = line == range.end.line ? range.end.column : m_lines[line].size();
            callback(line, start, end);
        }
    }

    // Convert codepoint index to text editor position (line:column).
    TextPosition index_to_position(size_t) const;

protected:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    void set_content_impl(Util::UString const&);
    TextPosition real_cursor_position() const;

private:
    virtual Widget::EventHandlerResult on_text_input(Event::TextInput const& event) override;
    virtual Widget::EventHandlerResult on_key_press(Event::KeyPress const& event) override;
    virtual Widget::EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const& event) override;
    virtual Widget::EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const& event) override;
    virtual Widget::EventHandlerResult on_mouse_move(Event::MouseMove const& event) override;

    virtual llgl::Cursor const& cursor(Util::Point2i /*local_position*/) const override { return llgl::Cursor::system(llgl::SystemCursor::IBeam); }

    TextPosition text_position_at(Util::Point2i);
    Util::Point2i calculate_cursor_position() const;
    void erase_selected_text();
    virtual void update() override;
    virtual bool accepts_focus() const override { return true; }
    virtual bool steals_focus() const override { return m_multiline; }
    virtual bool can_insert_codepoint(uint32_t) const { return true; }
    virtual void on_content_change() { }
    void did_content_change();

    int line_height() const;
    int left_margin() const;
    int character_width() const;
    virtual Util::Size2i content_size() const override;
    virtual LengthVector initial_size() const override;
    virtual Util::Recti scrollable_rect() const override;

    // sf::Clock m_cursor_clock;

    enum class CursorDirection { Left, Right };
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
    std::vector<ErrorSpan> m_error_spans;
    bool m_content_changed = false;
    bool m_multiline = true;
    bool m_editable = true;
};

}
