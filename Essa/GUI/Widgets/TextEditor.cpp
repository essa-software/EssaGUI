#include "TextEditor.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/TextEditing/SyntaxHighlighter.hpp>
#include <Essa/GUI/Widgets/ScrollableWidget.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/Window/Clipboard.hpp>
#include <EssaUtil/CharacterType.hpp>
#include <EssaUtil/DelayedInit.hpp>
#include <EssaUtil/UStringBuilder.hpp>

#include <sstream>
#include <string>

namespace GUI {

// FIXME: There is a bunch of narrowing conversions in this file.

TextEditor::TextEditor() { m_lines.push_back(""); }

int TextEditor::line_height() const { return Application::the().fixed_width_font().line_height(theme().label_font_size); }

int TextEditor::left_margin() const {
    return static_cast<int>(m_multiline ? theme().text_editor_gutter_width + theme().text_editor_margin : theme().text_editor_margin);
}

Util::Cs::Size2i TextEditor::content_size() const {
    auto line_height = this->line_height();
    int first_visible_line = std::min<int>(-scroll_offset().y() / line_height, static_cast<int>(m_lines.size()) - 1);
    int last_visible_line = std::min<int>((raw_size().y() - scroll_offset().y() / line_height), static_cast<int>(m_lines.size()) - 1);

    int width = 0;
    for (int s = first_visible_line; s <= last_visible_line; s++) {
        int line_width = static_cast<int>(m_lines[s].size()) * character_width();
        if (line_width > width) {
            width = line_width;
        }
    }

    return { width + 9, line_count() * line_height + 8 };
}

Util::Recti TextEditor::scrollable_rect() const {
    auto rect = local_rect();
    rect.left += left_margin();
    rect.width -= left_margin();
    return rect;
}

LengthVector TextEditor::initial_size() const { return m_multiline ? LengthVector {} : LengthVector { Util::Length::Auto, 30.0_px }; }

TextPosition TextEditor::real_cursor_position() const {
    TextPosition position = m_cursor;
    if (position.column > line(position.line).size()) {
        position.column = line(position.line).size();
    }
    return position;
}

TextPosition TextEditor::text_position_at(Util::Cs::Point2i position) {
    if (line_count() == 0)
        return {};

    auto delta = position.to_vector();
    if (delta.x() < 0)
        return {};

    auto line = m_multiline ? (delta.y() - scroll_offset().y()) / line_height() : 0;
    if (line < 0)
        return {};
    if (static_cast<size_t>(line) >= line_count())
        return { .line = std::max<size_t>(1, line_count()) - 1, .column = std::max<size_t>(1, last_line().size() + 1) - 1 };

    int character_width = this->character_width();
    auto cursor = (delta.x() - scroll_offset().x() - left_margin()) / character_width;
    return { .line = static_cast<size_t>(line), .column = std::min(static_cast<size_t>(cursor), this->line(line).size()) };
}

int TextEditor::character_width() const {
    // We can just check the offset of 1st character because we use
    // a fixed width font. Normally we would need to iterate over characters
    // to find the nearest one.
    Gfx::Text text { "test", GUI::Application::the().fixed_width_font() };
    text.set_font_size(theme().label_font_size);
    return std::floor(text.find_character_position(1));
}

Util::UString TextEditor::content() const {
    Util::UStringBuilder builder;
    size_t content_length = 0;
    for (size_t s = 0; s < line_count(); s++) {
        content_length += line(s).size();
    }
    builder.reserve(content_length);
    for (size_t s = 0; s < line_count(); s++) {
        builder.append(line(s));
        if (s != line_count() - 1)
            builder.append('\n');
    }
    return builder.release_string();
}

void TextEditor::set_content(Util::UString content, NotifyUser notify_user) {
    set_content_impl(content);

    did_content_change();
    if (notify_user == NotifyUser::No) {
        on_content_change();
        regenerate_styles();
        m_content_changed = false;
    }
}

void TextEditor::set_multiline(bool m) {
    m_multiline = m;
    set_x_scrollbar_visible(m_multiline);
    set_y_scrollbar_visible(m_multiline);
}

void TextEditor::set_content_impl(Util::UString const& content) {
    m_lines.clear();

    content.for_each_line([&](std::span<uint32_t const> span) { m_lines.push_back(Util::UString { span }); });

    if (line_count() > 0) {
        if (m_cursor.line >= line_count())
            m_cursor.line = line_count() - 1;
        if (m_cursor.column > line(m_cursor.line).size())
            m_cursor.column = line(m_cursor.line).size();
    }
    else {
        m_cursor = {};
    }
    update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
    scroll_to_bottom();
}

void TextEditor::update_selection_after_set_cursor(SetCursorSelectionBehavior extend_selection) {
    if (extend_selection == SetCursorSelectionBehavior::Clear
        || (extend_selection != SetCursorSelectionBehavior::DontTouch && !m_shift_pressed))
        m_selection_start = real_cursor_position();

    int x_offset = static_cast<int>(real_cursor_position().column) * character_width() + scroll_offset().x();
    auto max_x = scrollable_rect().width - character_width();
    if (x_offset < 0) {
        set_scroll_x(scroll().x() + x_offset);
    }
    else if (x_offset > max_x) {
        set_scroll_x(scroll().x() + (x_offset - max_x));
    }

    int y_offset = static_cast<int>(real_cursor_position().line) * line_height() + scroll_offset().y();
    auto max_y = raw_size().y() - line_height() - 8;
    if (y_offset < 0) {
        set_scroll_y(scroll().y() + y_offset);
    }
    else if (y_offset > max_y) {
        set_scroll_y(scroll().y() + (y_offset - max_y));
    }

    // TODO
    // m_cursor_clock.restart();
}

Widget::EventHandlerResult TextEditor::on_text_input(Event::TextInput const& event) {
    ScrollableWidget::on_text_input(event);
    for (auto codepoint : event.text()) {
        insert_codepoint(codepoint);
    }
    return Widget::EventHandlerResult::Accepted;
}

Widget::EventHandlerResult TextEditor::on_key_press(Event::KeyPress const& event) {
    ScrollableWidget::on_key_press(event);
    m_shift_pressed = event.modifiers().shift;
    switch (event.code()) {
    case llgl::KeyCode::Left: {
        if (event.modifiers().ctrl) {
            move_cursor_by_word(CursorDirection::Left);
        }
        else {
            move_cursor(CursorDirection::Left);
        }
        return EventHandlerResult::Accepted;
    } break;
    case llgl::KeyCode::Right: {
        if (event.modifiers().ctrl) {
            move_cursor_by_word(CursorDirection::Right);
        }
        else {
            move_cursor(CursorDirection::Right);
        }
        return EventHandlerResult::Accepted;
    } break;
    case llgl::KeyCode::Up: {
        if (!m_multiline)
            break;
        if (m_cursor.line > 0)
            m_cursor.line--;
        update_selection_after_set_cursor();
        return EventHandlerResult::Accepted;
    } break;
    case llgl::KeyCode::Down: {
        if (!m_multiline)
            break;
        if (m_cursor.line < line_count() - 1)
            m_cursor.line++;
        update_selection_after_set_cursor();
        return EventHandlerResult::Accepted;
    } break;
    case llgl::KeyCode::A: {
        if (event.modifiers().ctrl) {
            m_cursor = { .line = line_count() - 1, .column = line(line_count() - 1).size() };
            m_selection_start = {};
        }
        break;
    }
    case llgl::KeyCode::X: {
        if (event.modifiers().ctrl) {
            auto selected_text = this->selected_text();
            llgl::Clipboard::set_string(selected_text);
            erase_selected_text();
        }
        break;
    }
    case llgl::KeyCode::C: {
        if (event.modifiers().ctrl) {
            auto selected_text = this->selected_text();
            llgl::Clipboard::set_string(selected_text);
        }
        break;
    }
    case llgl::KeyCode::V: {
        if (event.modifiers().ctrl && llgl::Clipboard::has_string()) {
            erase_selected_text();
            m_selection_start = real_cursor_position();
            for (auto codepoint : llgl::Clipboard::get_string())
                insert_codepoint(codepoint);
        }
        break;
    }
    case llgl::KeyCode::Enter: {
        // TODO: Handle multiline case
        if (m_multiline) {
            if (event.modifiers().ctrl && on_enter) {
                on_enter(content());
            }
            else {
                insert_codepoint('\n');
                if (real_cursor_position().line > 0) {
                    auto indent = line(real_cursor_position().line - 1).indent();
                    // Indent
                    for (size_t s = 0; s < indent; s++) {
                        insert_codepoint(' ');
                    }
                }
            }
        }
        else {
            if (on_enter)
                on_enter(content());
        }
        break;
    }
    case llgl::KeyCode::Tab: {
        if (!m_multiline)
            break;
        if (!can_insert_codepoint(' '))
            break;
        do {
            insert_codepoint(' ');
        } while (real_cursor_position().column % 4 != 0);
        break;
    }
    case llgl::KeyCode::Backspace: {
        if (line_count() > 0) {
            m_cursor = real_cursor_position();
            if (m_cursor == m_selection_start) {
                if (m_cursor.column > 0) {
                    if (event.modifiers().ctrl) {
                        auto old_cursor = m_cursor;
                        move_cursor_by_word(CursorDirection::Left);
                        m_lines[m_cursor.line] = m_lines[m_cursor.line].erase(m_cursor.column, old_cursor.column - m_cursor.column);
                    }
                    else {
                        auto remove_character = [this]() {
                            m_cursor.column--;
                            m_lines[m_cursor.line] = m_lines[m_cursor.line].erase(m_cursor.column);
                        };
                        if (isspace(m_lines[m_cursor.line].at(m_cursor.column - 1))) {
                            do {
                                remove_character();
                            } while (m_cursor.column > 0 && m_cursor.column % 4 != 0
                                     && isspace(m_lines[m_cursor.line].at(m_cursor.column - 1)));
                        }
                        else {
                            remove_character();
                        }
                    }
                }
                else if (m_cursor.line != 0) {
                    m_cursor.line--;
                    size_t old_size = m_lines[m_cursor.line].size();
                    if (m_cursor.line < line_count())
                        m_lines[m_cursor.line] = m_lines[m_cursor.line] + m_lines[m_cursor.line + 1];
                    m_lines.erase(m_lines.begin() + m_cursor.line + 1);
                    m_cursor.column = old_size;
                }
                update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
                did_content_change();
            }
            else {
                erase_selected_text();
            }
        }
        break;
    }
    case llgl::KeyCode::Delete: {
        if (line_count() > 0) {
            m_cursor = real_cursor_position();
            if (m_cursor == m_selection_start) {
                if (m_cursor.column < m_lines[m_cursor.line].size()) {
                    if (event.modifiers().ctrl) {
                        auto old_cursor = m_cursor;
                        move_cursor_by_word(CursorDirection::Right);
                        m_lines[m_cursor.line] = m_lines[m_cursor.line].erase(old_cursor.column, m_cursor.column - old_cursor.column);
                        m_cursor = old_cursor;
                    }
                    else {
                        m_lines[m_cursor.line] = m_lines[m_cursor.line].erase(m_cursor.column);
                    }
                }
                else if (m_cursor.line < line_count() - 1) {
                    m_lines[m_cursor.line] = m_lines[m_cursor.line] + m_lines[m_cursor.line + 1];
                    m_lines.erase(m_lines.begin() + m_cursor.line + 1);
                }
                update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
                did_content_change();
            }
            else {
                erase_selected_text();
            }
        }
        break;
    }
    case llgl::KeyCode::Home: {
        m_cursor.column = 0;
        update_selection_after_set_cursor();
        break;
    }
    case llgl::KeyCode::End: {
        m_cursor.column = m_lines[m_cursor.line].size();
        update_selection_after_set_cursor();
        break;
    }
    default:
        break;
    }
    return EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult TextEditor::on_mouse_button_press(Event::MouseButtonPress const& event) {
    ScrollableWidget::on_mouse_button_press(event);
    m_cursor = text_position_at(event.local_position());
    update_selection_after_set_cursor();
    m_dragging = true;
    return EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult TextEditor::on_mouse_button_release(Event::MouseButtonRelease const& event) {
    ScrollableWidget::on_mouse_button_release(event);
    m_dragging = false;
    return EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult TextEditor::on_mouse_move(Event::MouseMove const& event) {
    if (!is_focused()) {
        return EventHandlerResult::NotAccepted;
    }
    ScrollableWidget::on_mouse_move(event);
    if (m_dragging) {
        m_cursor = text_position_at(event.local_position());
        update_selection_after_set_cursor(SetCursorSelectionBehavior::DontTouch);
    }
    return EventHandlerResult::NotAccepted;
}

Util::UString TextEditor::selected_text() const {
    auto selection_start = std::min(m_selection_start, real_cursor_position());
    auto selection_end = std::max(m_selection_start, real_cursor_position());

    if (selection_start.line == selection_end.line) {
        return m_lines[selection_start.line].substring(selection_start.column, selection_end.column - selection_start.column);
    }

    Util::UString text;
    for_each_line_in_range({ selection_start, selection_end }, [&](size_t line, size_t start, size_t end) {
        text = text + m_lines[line].substring(start, end - start);
        if (line != selection_end.line)
            text = text + "\n";
    });

    return text;
}

void TextEditor::erase_selected_text() {
    m_cursor = real_cursor_position();
    if (m_selection_start != m_cursor) {
        auto selection_start = std::min(m_selection_start, m_cursor);
        auto selection_end = std::max(m_selection_start, m_cursor);
        auto after_selection_line_part = m_lines[selection_end.line].substring(selection_end.column);
        if (selection_start.line == selection_end.line) {
            m_lines[selection_start.line]
                = m_lines[selection_start.line].erase(selection_start.column, selection_end.column - selection_start.column);
        }
        else {
            for (size_t s = selection_start.line; s <= selection_end.line; s++) {
                float start = s == selection_start.line ? selection_start.column : 0;
                float end = s == selection_end.line ? selection_end.column : m_lines[s].size();
                m_lines[s] = m_lines[s].erase(start, end - start + 1);
            }
            m_lines.erase(m_lines.begin() + selection_start.line + 1, m_lines.begin() + selection_end.line + 1);
            m_lines[selection_start.line] = m_lines[selection_start.line] + after_selection_line_part;
        }
        m_cursor = selection_start;
    }
    did_content_change();
    update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
}

void TextEditor::move_cursor(CursorDirection direction) {
    m_cursor = real_cursor_position();
    if (m_cursor != m_selection_start && !m_shift_pressed) {
        if ((direction == CursorDirection::Right) != (m_cursor < m_selection_start))
            m_selection_start = m_cursor;
        else
            m_cursor = m_selection_start;
        return;
    }
    switch (direction) {
    case CursorDirection::Left:
        if (m_cursor.column == 0) {
            if (m_cursor.line == 0)
                break;
            m_cursor.line--;
            m_cursor.column = m_lines[m_cursor.line].size();
        }
        else
            m_cursor.column--;
        break;
    case CursorDirection::Right:
        if (m_cursor.column == m_lines[m_cursor.line].size()) {
            if (m_cursor.line == line_count() - 1)
                break;
            m_cursor.line++;
            m_cursor.column = 0;
        }
        else
            m_cursor.column++;
        break;
    }

    update_selection_after_set_cursor();
}

void TextEditor::move_cursor_by_word(CursorDirection direction) {
    if (line_count() == 0)
        return;

    // Trying to mimic vscode behavior in c++ source files.
    enum class State { Start, PendingPunctuation, PendingCharactersOfType, Done } state = State::Start;
    Util::CharacterType last_character_type = Util::CharacterType::Unknown;

    auto cursor = real_cursor_position();

    if (direction == CursorDirection::Left && cursor.column == 0)
        move_cursor(CursorDirection::Left);
    else if (direction == CursorDirection::Right && cursor.column == m_lines[cursor.line].size())
        move_cursor(CursorDirection::Right);

    auto content = m_lines[cursor.line];

    auto is_in_range = [&](unsigned offset) {
        return (direction == CursorDirection::Left && offset > 0) || (direction == CursorDirection::Right && offset < content.size());
    };

    auto new_cursor = cursor.column;
    while (state != State::Done && is_in_range(new_cursor)) {
        auto next = [&]() -> uint32_t {
            if (direction == CursorDirection::Right && new_cursor + 1 == content.size())
                return 0;
            return content.at(direction == CursorDirection::Left ? new_cursor - 1 : new_cursor + 1);
        }();
        // std::cout << "'" << (char)next << "' " << (int)state << " : " <<
        // ispunct(next) << std::endl;
        switch (state) {
        case State::Start:
            if (ispunct(next))
                state = State::PendingCharactersOfType;
            else if (!isspace(next)) {
                if (direction == CursorDirection::Left && !is_in_range(new_cursor - 1))
                    break;
                if (direction == CursorDirection::Right && !is_in_range(new_cursor + 1))
                    break;
                if (ispunct(content.at(direction == CursorDirection::Left ? new_cursor - 1 : new_cursor + 1)))
                    state = State::PendingPunctuation;
                else
                    state = State::PendingCharactersOfType;
                new_cursor++;
            }
            break;
        case State::PendingPunctuation:
            assert(ispunct(next));
            state = State::PendingCharactersOfType;
            break;
        case State::PendingCharactersOfType: {
            auto next_type = Util::character_type(next);
            if (next_type != last_character_type && last_character_type != Util::CharacterType::Unknown) {
                state = State::Done;
                if (direction == CursorDirection::Left)
                    new_cursor++;
            }
            last_character_type = next_type;
            break;
        }
        default:
            assert(false);
            break;
        }
        if (direction == CursorDirection::Left)
            new_cursor--;
        else if (new_cursor < content.size())
            new_cursor++;
    }

    m_cursor.column = new_cursor;
    update_selection_after_set_cursor();
}

void TextEditor::insert_codepoint(uint32_t codepoint) {
    if (!can_insert_codepoint(codepoint))
        return;
    m_cursor = real_cursor_position();
    if ((codepoint == '\r' || codepoint == '\n')) {
        if (m_multiline) {
            auto line = m_lines[m_cursor.line];
            auto old_part = line.substring(0, m_cursor.column);
            auto new_part = line.substring(m_cursor.column);
            m_lines.insert(m_lines.begin() + m_cursor.line + 1, new_part);
            m_lines[m_cursor.line] = old_part;
            m_cursor.line++;
            m_cursor.column = 0;
        }
        else {
            return;
        }
    }
    else if (codepoint >= 0x20) {
        if (m_cursor != m_selection_start)
            erase_selected_text();
        m_lines[m_cursor.line] = m_lines[m_cursor.line].insert(Util::UString { codepoint }, m_cursor.column);
        m_cursor.column++;
    }
    else {
        return;
    }
    update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
    did_content_change();
}

Util::Cs::Point2i TextEditor::calculate_cursor_position() const {
    auto cursor = real_cursor_position();
    Gfx::Text text { m_lines[cursor.line], GUI::Application::the().fixed_width_font() };
    text.set_font_size(theme().label_font_size);
    auto character_position = text.find_character_position(cursor.column);
    auto position = Util::Cs::Point2i { character_position, 0 } + scroll_offset();
    auto const cursor_height = std::min<int>(raw_size().y() - 6, line_height());
    if (m_multiline)
        position.set_y(position.y() + line_height() / 2 - cursor_height / 4 + line_height() * static_cast<int>(cursor.line));
    else
        position.set_y(raw_size().y() / 2 - cursor_height / 2);
    return position;
}

void TextEditor::update() {
    if (m_content_changed) {
        regenerate_styles();
        on_content_change();
        if (on_change) {
            on_change(content());
        }
        m_content_changed = false;
    }
}

void TextEditor::regenerate_styles() {
    auto content = this->content();

    // Reset styles array to defaults
    std::fill(m_styles_for_letter.begin(), m_styles_for_letter.end(), std::optional<size_t> {});
    m_styles_for_letter.resize(content.size());

    // Update styles with a syntax highlighter
    if (m_syntax_highlighter) {
        if (m_styles.empty()) {
            m_styles = m_syntax_highlighter->styles();
        }
        auto highlighter_spans = m_syntax_highlighter->spans(content);
        for (auto const& span : highlighter_spans) {
            for (size_t s = 0; s < span.span_size; s++) {
                if (span.span_start + s >= m_styles_for_letter.size()) {
                    break;
                }
                m_styles_for_letter[span.span_start + s] = span.style_index;
            }
        }
    }

    m_error_spans.clear();
}

void TextEditor::did_content_change() { m_content_changed = true; }

TextPosition TextEditor::index_to_position(size_t offset) const {
    TextPosition position;

    while (offset >= m_lines[position.line].size() + 1) {
        position.line++;
        offset -= m_lines[position.line].size() + 1;
    }

    position.column = offset;
    return position;
}

static void draw_error_line(Gfx::Painter& painter, TextEditor::ErrorSpan::Type type, Util::Vector2f start, float width) {

    int CurlyHeights[] = { 0, -1, -1, 0, 1, 1 };

    auto draw_curly = [&](Util::Vector2f position, float width, Util::Color color) {
        std::vector<Gfx::Vertex> vertices;
        for (int x = position.x(); x < position.x() + width; x += 1) {
            int y = position.y() + CurlyHeights[x % 6];
            vertices.push_back(Gfx::Vertex { { x, y }, color, {} });
        }
        painter.draw_vertices(llgl::PrimitiveType::Points, vertices);
    };

    switch (type) {
    case TextEditor::ErrorSpan::Type::Note:
        draw_curly(start, width, Util::Colors::Gray);
        break;
    case TextEditor::ErrorSpan::Type::Warning:
        draw_curly(start, width, Util::Colors::Orange);
        break;
    case TextEditor::ErrorSpan::Type::Error:
        draw_curly(start, width, Util::Colors::Red);
        break;
    }
}

void TextEditor::draw(Gfx::Painter& painter) const {
    auto cursor = real_cursor_position();
    auto theme_colors = theme().textbox.value(*this);

    theme().renderer().draw_text_editor_background(*this, painter);

    if (m_multiline) {
        Gfx::RectangleDrawOptions gutter_rect;
        gutter_rect.fill_color = theme().gutter.background;
        painter.deprecated_draw_rectangle({ {}, { theme().text_editor_gutter_width, raw_size().y() } }, gutter_rect);
    }

    auto clip_rect = scrollable_rect();

    auto const cursor_height = std::min(raw_size().y() - 6, line_height());
    if (m_selection_start != cursor) {
        Util::DelayedInit<Gfx::ClipViewScope> scope;
        if (m_multiline) {
            scope.construct(painter, clip_rect, Gfx::ClipViewScope::Mode::Intersect);
        }

        Gfx::RectangleDrawOptions selected_rect;
        selected_rect.fill_color = theme().selection.value(*this);
        auto selection_start = std::min(m_selection_start, cursor);
        auto selection_end = std::max(m_selection_start, cursor);
        for_each_line_in_range({ selection_start, selection_end }, [&](size_t line, size_t start, size_t end) {
            Gfx::Text text { m_lines[line], Application::the().fixed_width_font() };
            text.set_font_size(theme().label_font_size);
            float start_x = text.find_character_position(start);
            float end_x = text.find_character_position(end);
            float y = m_multiline ? line_height() / 2 - cursor_height / 4 + line_height() * line : raw_size().y() / 2 - cursor_height / 2;
            painter.deprecated_draw_rectangle(
                {
                    Util::Cs::Point2f(start_x + (m_multiline ? 0 : left_margin()), y) + scroll_offset().cast<float>(),
                    { end_x - start_x, cursor_height },
                },
                selected_rect
            );
        });
    }

    Gfx::Text text { "", GUI::Application::the().fixed_width_font() };
    bool should_draw_placeholder = is_empty();
    text.set_fill_color(should_draw_placeholder ? theme().placeholder : theme_colors.text);
    text.set_font_size(theme().label_font_size);

    TextStyle const default_style { .color = theme_colors.text };

    auto line_height = this->line_height();
    size_t first_visible_line = std::min(static_cast<size_t>(-scroll_offset().y() / line_height), m_lines.size() - 1);
    size_t last_visible_line
        = std::min(static_cast<size_t>((scroll_area_size().y() - scroll_offset().y()) / line_height), m_lines.size() - 1);

    {
        if (!m_multiline) {
            Util::Recti align_rect { static_cast<int>(theme().text_editor_margin), 0, raw_size().x(), raw_size().y() };
            align_rect = align_rect.move_x(scroll_offset().x());
            text.set_string(should_draw_placeholder ? m_placeholder : m_lines[0]);
            assert(should_draw_placeholder || line_count() > 0);
            text.align(Align::CenterLeft, align_rect.cast<float>());
            text.draw(painter);
        }
        else {
            if (should_draw_placeholder) {
                auto position = scroll_offset().to_point();
                position += { left_margin(), line_height };
                text.set_string(m_placeholder);
                text.set_position(position.cast<float>());
                text.draw(painter);
            }
            else {
                // Text
                Gfx::ClipViewScope scope { painter, Util::Recti { clip_rect }, Gfx::ClipViewScope::Mode::Intersect };

                size_t character_index = 0;
                for (size_t i = 0; i < first_visible_line; i++) {
                    character_index += line(i).size() + 1;
                }

                auto character_width = this->character_width();

                auto position = scroll_offset().to_point();
                position.set_y(position.y() + line_height * first_visible_line);

                for (size_t i = first_visible_line; i <= last_visible_line; i++) {
                    auto const& line = m_lines[i];
                    position.set_x(scroll_offset().x());
                    position.set_y(position.y() + line_height);
                    for (auto character : line) {
                        auto style_idx = m_styles_for_letter[character_index];
                        auto const& style = style_idx ? m_styles[*style_idx] : default_style;
                        text.set_string(Util::UString { character });
                        text.set_fill_color(style.color);
                        text.set_position(position.cast<float>());
                        text.draw(painter);
                        position.set_x(position.x() + character_width);
                        character_index++;
                    }
                    character_index++; // Newline
                }

                // Errors
                for (auto const& error : m_error_spans) {
                    Util::Vector2f base_position { scroll_offset().x(), scroll_offset().y() + line_height };
                    for_each_line_in_range(error.range, [&](size_t line, size_t start, size_t end) {
                        Util::Vector2f start_position { base_position.x() + start * character_width,
                                                        base_position.y() + line * line_height + 3 };
                        draw_error_line(painter, error.type, start_position, (end - start) * character_width);
                    });
                }
            }
        }
    }

    // Line numbers
    if (m_multiline) {
        Gfx::Text text { "", GUI::Application::the().fixed_width_font() };
        Util::Cs::Point2f position;
        position.set_y(5 + scroll_offset().y() + line_height * static_cast<int>(first_visible_line));
        text.set_fill_color(theme().gutter.text);
        text.set_font_size(theme().label_font_size);

        for (size_t i = first_visible_line; i <= last_visible_line; i++) {
            text.set_string(Util::UString { std::to_string(i + 1) });
            text.align(Align::CenterRight, { position, { theme().text_editor_gutter_width - 10, line_height } });
            text.draw(painter);
            position.set_y(position.y() + line_height);
        }
    }

    if (is_focused()) {
        // if ((m_cursor_clock.getElapsedTime().asMilliseconds() / 500) % 2 ==
        // 0) {
        auto position = calculate_cursor_position();
        Gfx::RectangleDrawOptions cursor;
        cursor.fill_color = theme_colors.text;
        painter.deprecated_draw_rectangle(
            { (position + Util::Cs::Vector2i { left_margin(), 0 }).cast<float>(), { 2, cursor_height } }, cursor
        );
        // }
    }

    // Border
    theme().renderer().draw_text_editor_border(*this, is_multiline(), painter);
    ScrollableWidget::draw_scrollbar(painter);
}

EML::EMLErrorOr<void> TextEditor::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));
    m_placeholder = TRY(object.get_property("placeholder", EML::Value("")).to_string());
    m_multiline = TRY(object.get_property("multiline", EML::Value(false)).to_bool());
    return {};
}

EML_REGISTER_CLASS(TextEditor);
}
