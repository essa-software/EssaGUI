#include "TextEditor.hpp"
#include "EssaGUI/gui/ScrollableWidget.hpp"

#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/util/CharacterType.hpp>

namespace GUI {

constexpr int FontSize = 15;

float TextEditor::line_height() const {
    return Application::the().fixed_width_font.getLineSpacing(FontSize);
}

float TextEditor::content_height() const {
    return m_lines.size() * line_height() + 5;
}

TextPosition TextEditor::m_character_pos_from_mouse(Event& event) {
    if (m_lines.size() == 0)
        return {};

    auto delta = event.mouse_position() - position();
    if (delta.x < 0)
        return {};

    auto line = (delta.y - scroll_offset().y) / line_height();
    if (line < 0)
        return {};
    if (line >= m_lines.size())
        return { .line = std::max<size_t>(1, m_lines.size()) - 1, .column = std::max<size_t>(1, m_lines.back().getSize() + 1) - 1 };

    // We can just check the offset of 1st character because we use
    // a fixed width font. Normally we would need to iterate over characters
    // to find the nearest one.
    float character_width = window().find_character_position(1, "test", GUI::Application::the().fixed_width_font, get_text_options()).x;
    auto cursor = (delta.x - scroll_offset().x) / character_width;
    return { .line = static_cast<size_t>(line), .column = std::min(static_cast<size_t>(cursor), m_lines[line].getSize()) };
}

sf::String TextEditor::get_content() const {
    sf::String content;
    for (size_t s = 0; s < m_lines.size(); s++) {
        content += m_lines[s];
        if (s != m_lines.size() - 1)
            content += '\n';
    }
    return content;
}

void TextEditor::set_content(sf::String, NotifyUser) {
    // TODO
}

void TextEditor::update_selection_after_set_cursor(SetCursorSelectionBehavior extend_selection) {
    if (extend_selection == SetCursorSelectionBehavior::Clear || (extend_selection != SetCursorSelectionBehavior::DontTouch && !m_shift_pressed))
        m_selection_start = m_cursor;

    // TODO: Handle X scroll
    auto y_offset = m_cursor.line * line_height() + scroll_offset().y;
    std::cout << y_offset << std::endl;
    auto max_y = size().y - line_height() - 8;
    if (y_offset < 0)
        set_scroll(scroll() + y_offset);
    else if (y_offset > max_y)
        set_scroll(scroll() + (y_offset - max_y));
}

void TextEditor::handle_event(Event& event) {
    ScrollableWidget::handle_event(event);

    if (event.type() == sf::Event::TextEntered) {
        if (is_focused()) {
            auto codepoint = event.event().text.unicode;
            if (codepoint == '\b' && m_lines.size() > 0) {
                std::cout << m_cursor.line << ":" << m_cursor.column << std::endl;
                if (m_cursor == m_selection_start) {
                    if (m_cursor.column > 0) {
                        m_cursor.column--;
                        m_lines[m_cursor.line].erase(m_cursor.column);
                    }
                    else if (m_cursor.line != 0) {
                        m_cursor.line--;
                        size_t old_size = m_lines[m_cursor.line].getSize();
                        if (m_cursor.line < m_lines.size())
                            m_lines[m_cursor.line] += m_lines[m_cursor.line + 1];
                        m_lines.erase(m_lines.begin() + m_cursor.line + 1);
                        m_cursor.column = old_size;
                    }
                    update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
                }
                else {
                    erase_selected_text();
                }
                if (on_change)
                    on_change(get_content());
            }
            else if (codepoint == 0x7f && m_lines.size() > 0) {
                if (m_cursor == m_selection_start) {
                    if (m_cursor.column < m_lines[m_cursor.line].getSize())
                        m_lines[m_cursor.line].erase(m_cursor.column);
                    else if (m_cursor.line < m_lines.size() - 1) {
                        m_lines[m_cursor.line] += m_lines[m_cursor.line + 1];
                        m_lines.erase(m_lines.begin() + m_cursor.line + 1);
                    }
                }
                else {
                    erase_selected_text();
                }
                if (on_change)
                    on_change(get_content());
            }
            else {
                insert_codepoint(codepoint);
            }
            event.set_handled();
        }
    }
    else if (event.type() == sf::Event::KeyPressed) {
        m_shift_pressed = event.event().key.shift;
        if (is_focused()) {
            // FIXME: Focus check should be handled at Widget level.
            switch (event.event().key.code) {
            case sf::Keyboard::Left: {
                if (event.event().key.control) {
                    move_cursor_by_word(CursorDirection::Left);
                }
                else {
                    move_cursor(CursorDirection::Left);
                }
                event.set_handled();
            } break;
            case sf::Keyboard::Right: {
                if (event.event().key.control) {
                    move_cursor_by_word(CursorDirection::Right);
                }
                else {
                    move_cursor(CursorDirection::Right);
                }
                event.set_handled();
            } break;
            case sf::Keyboard::Up: {
                if (m_cursor.line > 0)
                    m_cursor.line--;
                if (m_cursor.column > m_lines[m_cursor.line].getSize())
                    m_cursor.column = m_lines[m_cursor.line].getSize();
                update_selection_after_set_cursor();
                event.set_handled();
            } break;
            case sf::Keyboard::Down: {
                if (m_cursor.line < m_lines.size() - 1)
                    m_cursor.line++;
                if (m_cursor.column > m_lines[m_cursor.line].getSize())
                    m_cursor.column = m_lines[m_cursor.line].getSize();
                update_selection_after_set_cursor();
                event.set_handled();
            } break;
            case sf::Keyboard::A: {
                if (event.event().key.control) {
                    m_cursor = { .line = m_lines.size() - 1, .column = m_lines[m_lines.size() - 1].getSize() };
                    m_selection_start = {};
                }
                break;
            }
            case sf::Keyboard::X: {
                if (event.event().key.control) {
                    sf::Clipboard::setString(selected_text());
                    erase_selected_text();
                }
                break;
            }
            case sf::Keyboard::C: {
                if (event.event().key.control) {
                    sf::Clipboard::setString(selected_text());
                }
                break;
            }
            case sf::Keyboard::V: {
                if (event.event().key.control) {
                    erase_selected_text();
                    m_selection_start = m_cursor;
                    auto clipboard_contents = sf::Clipboard::getString();
                    for (auto codepoint : clipboard_contents)
                        insert_codepoint(codepoint);
                }
                break;
            }
            case sf::Keyboard::Enter: {
                // TODO: Handle multiline case
                if (on_enter)
                    on_enter(get_content());
                break;
            }
            default:
                break;
            }
        }
    }
    else if (event.type() == sf::Event::MouseButtonPressed) {
        if (is_hover()) {
            m_cursor = m_character_pos_from_mouse(event);
            update_selection_after_set_cursor();
            m_dragging = true;
        }
    }
    else if (event.type() == sf::Event::MouseButtonReleased) {
        m_dragging = false;
    }
    else if (event.type() == sf::Event::MouseMoved) {
        if (m_dragging) {
            m_cursor = m_character_pos_from_mouse(event);
            update_selection_after_set_cursor(SetCursorSelectionBehavior::DontTouch);
        }
    }
}

sf::String TextEditor::selected_text() const {
    auto selection_start = std::min(m_selection_start, m_cursor);
    auto selection_end = std::max(m_selection_start, m_cursor);

    if (selection_start.line == selection_end.line) {
        return m_lines[selection_start.line].substring(selection_start.column, selection_end.column - selection_start.column);
    }

    sf::String text;
    for (size_t s = selection_start.line; s <= selection_end.line; s++) {
        float start = s == selection_start.line ? selection_start.column : 0;
        float end = s == selection_end.line ? selection_end.column : m_lines[s].getSize();
        text += m_lines[s].substring(start, end - start + 1);
    }
    return text;
}

void TextEditor::erase_selected_text() {
    if (m_selection_start != m_cursor) {
        auto selection_start = std::min(m_selection_start, m_cursor);
        auto selection_end = std::max(m_selection_start, m_cursor);
        auto after_selection_line_part = m_lines[selection_end.line].substring(selection_end.column);
        if (selection_start.line == selection_end.line) {
            m_lines[selection_start.line].erase(selection_start.column, selection_end.column - selection_start.column);
        }
        else {
            for (size_t s = selection_start.line; s <= selection_end.line; s++) {
                float start = s == selection_start.line ? selection_start.column : 0;
                float end = s == selection_end.line ? selection_end.column : m_lines[s].getSize();
                m_lines[s].erase(start, end - start + 1);
            }
            m_lines.erase(m_lines.begin() + selection_start.line + 1, m_lines.begin() + selection_end.line + 1);
            m_lines[selection_start.line] += after_selection_line_part;
        }
        m_cursor = selection_start;
    }
    update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
}

void TextEditor::move_cursor(CursorDirection direction) {
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
            m_cursor.column = m_lines[m_cursor.line].getSize();
        }
        else
            m_cursor.column--;
        break;
    case CursorDirection::Right:
        if (m_cursor.column == m_lines[m_cursor.line].getSize()) {
            if (m_cursor.line == m_lines.size() - 1)
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
    if (m_lines.size() == 0)
        return;

    // Trying to mimic vscode behavior in c++ source files.
    enum class State {
        Start,
        PendingPunctuation,
        PendingCharactersOfType,
        Done
    } state
        = State::Start;
    Util::CharacterType last_character_type = Util::CharacterType::Unknown;

    if (direction == CursorDirection::Left && m_cursor.column == 0)
        move_cursor(CursorDirection::Left);
    else if (direction == CursorDirection::Right && m_cursor.column == m_lines[m_cursor.line].getSize())
        move_cursor(CursorDirection::Right);

    auto content = m_lines[m_cursor.line];

    auto is_in_range = [&](unsigned offset) {
        return (direction == CursorDirection::Left && offset > 0) || (direction == CursorDirection::Right && offset < content.getSize());
    };

    auto new_cursor = m_cursor.column;
    while (state != State::Done && is_in_range(new_cursor)) {
        auto next = content[direction == CursorDirection::Left ? new_cursor - 1 : new_cursor + 1];
        // std::cout << "'" << (char)next << "' " << (int)state << " : " << ispunct(next) << std::endl;
        switch (state) {
        case State::Start:
            if (ispunct(next))
                state = State::PendingCharactersOfType;
            else if (!isspace(next)) {
                if (is_in_range(new_cursor - 2) && ispunct(content[direction == CursorDirection::Left ? new_cursor - 1 : new_cursor + 1]))
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
        else if (new_cursor < content.getSize())
            new_cursor++;
    }

    m_cursor.column = new_cursor;
    update_selection_after_set_cursor();
}

void TextEditor::insert_codepoint(uint32_t codepoint) {
    if (codepoint == '\r' || codepoint == '\n') {
        if (m_lines.empty())
            m_lines.push_back("");
        else {
            auto line = m_lines[m_cursor.line];
            auto old_part = line.substring(0, m_cursor.column);
            auto new_part = line.substring(m_cursor.column);
            m_lines.insert(m_lines.begin() + m_cursor.line + 1, new_part);
            m_lines[m_cursor.line] = old_part;
            m_cursor.line++;
            m_cursor.column = 0;
            update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
        }
    }
    else if (codepoint >= 0x20) {

        if (m_cursor != m_selection_start)
            erase_selected_text();
        if (m_lines.empty())
            m_lines.push_back("");
        m_lines[m_cursor.line].insert(m_cursor.column, codepoint);
        if (on_change)
            on_change(get_content());
        m_cursor.column++;
        update_selection_after_set_cursor(SetCursorSelectionBehavior::Clear);
    }
}

sf::Vector2f TextEditor::calculate_cursor_position() const {
    if (m_lines.empty())
        return {};
    auto options = get_text_options();
    auto position = window().find_character_position(m_cursor.column, m_lines[m_cursor.line], GUI::Application::the().fixed_width_font, options)
        + scroll_offset();
    auto const cursor_height = std::min(size().y - 6, line_height());
    position.y += line_height() / 2 - cursor_height / 4 + line_height() * m_cursor.line;
    return position;
}

TextDrawOptions TextEditor::get_text_options() const {
    TextDrawOptions options;
    options.font_size = FontSize;
    options.fill_color = are_all_parents_enabled() ? theme().active_textbox.text : theme().textbox.text;

    if (!is_focused() && m_lines.empty())
        options.fill_color = theme().placeholder;
    return options;
}

void TextEditor::draw(GUI::SFMLWindow& window) const {
    RectangleDrawOptions rect;
    rect.outline_color = sf::Color(80, 80, 80);
    rect.outline_thickness = -2;
    rect.fill_color = are_all_parents_enabled() ? theme().active_textbox.background : theme().textbox.background;

    if (is_focused())
        rect.outline_color = are_all_parents_enabled() ? theme().active_textbox.foreground : theme().textbox.foreground;

    window.draw_rectangle(local_rect(), rect);

    auto const cursor_height = std::min(size().y - 6, line_height());

    if (m_selection_start != m_cursor) {
        RectangleDrawOptions selected_rect;
        selected_rect.fill_color = is_focused() ? theme().active_selection : theme().selection;
        auto selection_start = std::min(m_selection_start, m_cursor);
        auto selection_end = std::max(m_selection_start, m_cursor);
        for (size_t s = selection_start.line; s <= selection_end.line; s++) {
            float start = window.find_character_position(s == selection_start.line ? selection_start.column : 0, m_lines[s],
                                    Application::the().fixed_width_font,
                                    get_text_options())
                              .x;
            float end = window.find_character_position(s == selection_end.line ? selection_end.column : m_lines[s].getSize(), m_lines[s],
                                  Application::the().fixed_width_font,
                                  get_text_options())
                            .x;
            window.draw_rectangle({ sf::Vector2f { start, line_height() / 2 - cursor_height / 4 + line_height() * s } + scroll_offset(), { end - start, cursor_height } }, selected_rect);
        }
    }
    TextDrawOptions text_options = get_text_options();
    sf::Vector2f position = scroll_offset();
    for (auto& line : m_lines) {
        position.y += line_height();
        window.draw_text(line, Application::the().fixed_width_font, position, text_options);
    }

    if (is_focused()) {
        // FIXME: Reset timer when writing/selecting etc
        if ((m_cursor_clock.getElapsedTime().asMilliseconds() / 500) % 2 == 0) {
            auto position = calculate_cursor_position();
            RectangleDrawOptions cursor;
            cursor.fill_color = sf::Color::Black;
            window.draw_rectangle({ position, sf::Vector2f(2, cursor_height) },
                cursor);
        }
    }

    // Border once again so that it covers text
    rect.fill_color = sf::Color::Transparent;
    window.draw_rectangle(local_rect(), rect);

    ScrollableWidget::draw_scrollbar(window);
}
}
