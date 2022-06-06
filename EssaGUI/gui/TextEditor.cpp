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
    return m_lines.size() * line_height();
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
    return { .line = static_cast<size_t>(line), .column = std::min(static_cast<size_t>(cursor), m_lines.back().getSize()) };
}

sf::String TextEditor::get_content() const {
    // TODO
    return "";
}

void TextEditor::set_content(sf::String, NotifyUser) {
    // TODO
}

void TextEditor::update_selection_after_set_cursor(SetCursorSelectionBehavior extend_selection) {
    if (extend_selection == SetCursorSelectionBehavior::Clear || (extend_selection != SetCursorSelectionBehavior::DontTouch && !m_shift_pressed))
        m_selection_start = m_cursor;

    // TODO: Handle scroll
    // auto new_cursor_position = calculate_cursor_position();
    // if (new_cursor_position.x < 2)
    //     m_scroll += 2 - new_cursor_position.x;
    // if (new_cursor_position.x > size().x - 4)
    //     m_scroll += size().x - new_cursor_position.x - 4;
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
            else if (codepoint == '\r') {
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
            else if (codepoint != '\n') {
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
                else if (m_cursor == m_selection_start) {
                    move_cursor(CursorDirection::Left);
                }
                else if (!m_shift_pressed) {
                    if (m_cursor < m_selection_start)
                        m_selection_start = m_cursor;
                    else
                        m_cursor = m_selection_start;
                }
                event.set_handled();
            } break;
            case sf::Keyboard::Right: {
                if (event.event().key.control) {
                    move_cursor_by_word(CursorDirection::Right);
                }
                else if (m_cursor == m_selection_start) {
                    move_cursor(CursorDirection::Right);
                }
                else if (!m_shift_pressed) {
                    if (m_cursor > m_selection_start)
                        m_selection_start = m_cursor;
                    else
                        m_cursor = m_selection_start;
                }

                event.set_handled();
            } break;
            case sf::Keyboard::A: {
                if (event.event().key.control) {
                    m_cursor = { .line = m_lines.size() - 1, .column = m_lines[m_lines.size() - 1].getSize() };
                    m_selection_start = {};
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
                    m_selection_start = m_cursor;
                    auto clipboard_contents = sf::Clipboard::getString();
                    // TODO
                    // m_content.insert(m_cursor, clipboard_contents);
                    // m_cursor += clipboard_contents.getSize();
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
    // TODO
    return "";
    // auto start = std::min(m_cursor, m_selection_start);
    // auto size = std::max(m_cursor, m_selection_start) - start;
    // return m_content.substring(start, size);
}

void TextEditor::erase_selected_text() {
    // TODO
    // auto start = std::min(m_cursor, m_selection_start);
    // m_content.erase(start, std::max(m_cursor, m_selection_start) - start);
    // m_cursor = start;
    // m_selection_start = m_cursor;
}

void TextEditor::move_cursor(CursorDirection direction) {
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

void TextEditor::move_cursor_by_word(CursorDirection) {
    // TODO
}

sf::Vector2f TextEditor::calculate_cursor_position() const {
    if (m_lines.empty())
        return {};
    auto options = get_text_options();
    return window().find_character_position(m_cursor.column, m_lines[m_cursor.line], GUI::Application::the().fixed_width_font, options);
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

    // RectangleDrawOptions selected_rect;
    // selected_rect.fill_color = is_focused() ? theme().active_selection : theme().selection;
    // window.draw_rectangle({ { selection_start_pos, size().y / 2 - cursor_height / 2 }, { selection_end_pos - selection_start_pos, cursor_height } }, selected_rect);

    TextDrawOptions text_options = get_text_options();
    sf::Vector2f position;
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
            window.draw_rectangle({ { position.x, line_height() / 2 - cursor_height / 4 + line_height() * m_cursor.line },
                                      sf::Vector2f(2, cursor_height) },
                cursor);
        }
    }

    // Border once again so that it covers text
    rect.fill_color = sf::Color::Transparent;
    window.draw_rectangle(local_rect(), rect);

    ScrollableWidget::draw_scrollbar(window);
}
}
