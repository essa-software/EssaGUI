#include "Console.hpp"

#include "Application.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

constexpr float LINE_SPACING = 20;

void Console::handle_event(Event& event) {
    switch (event.type()) {
    case sf::Event::MouseWheelScrolled: {
        if (!is_hover() || event.event().mouseWheelScroll.wheel != sf::Mouse::VerticalWheel)
            break;
        event.set_handled();
        if (content_size() < scroll_area_size())
            break;
        m_scroll -= event.event().mouseWheelScroll.delta * 60;
        if (m_scroll < 0)
            m_scroll = 0;
        double bottom_content = m_lines.size() * LINE_SPACING - scroll_area_size();
        if (m_scroll > bottom_content)
            m_scroll = bottom_content;
    } break;
    default:
        break;
    }
}

void Console::append_line(LogLine line) {
    m_lines.push_back(line);
    if (size().y != 0) {
        double bottom_content = m_lines.size() * LINE_SPACING - scroll_area_size();
        if (bottom_content > 0)
            m_scroll = bottom_content;
    }
}

void Console::clear() {
    m_lines.clear();
}

constexpr float PADDING = 5;

void Console::draw(GUI::SFMLWindow& window) const {
    size_t s = 0;
    for (auto& line : m_lines) {
        TextDrawOptions options;
        options.fill_color = line.color;
        options.font_size = 15;
        window.draw_text(line.text, Application::the().fixed_width_font, { PADDING, s * LINE_SPACING - m_scroll + PADDING + 12 }, options);
        s++;
    }

    // "Scrollbar"
    float scroll_area_size = this->scroll_area_size();
    float content_size = this->content_size();
    if (content_size > scroll_area_size) {
        window.draw_rectangle({ { size().x - 3, m_scroll * scroll_area_size / content_size + PADDING }, { 3, scroll_area_size / content_size * scroll_area_size } });
    }
}

float Console::content_size() const {
    return m_lines.size() * LINE_SPACING;
}

float Console::scroll_area_size() const {
    return size().y - PADDING * 2;
}

}
