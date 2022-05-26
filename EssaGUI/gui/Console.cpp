#include "Console.hpp"

#include "Application.hpp"

#include <SFML/Graphics.hpp>

namespace GUI {

constexpr float LINE_SPACING = 20;

void Console::append_line(LogLine line) {
    m_lines.push_back(line);
    if (size().y != 0) {
        double bottom_content = m_lines.size() * LINE_SPACING - scroll_area_height();
        if (bottom_content > 0)
            set_scroll(bottom_content);
    }
}

void Console::clear() {
    m_lines.clear();
}

void Console::draw(GUI::SFMLWindow& window) const {
    size_t s = 0;
    for (auto& line : m_lines) {
        TextDrawOptions options;
        options.fill_color = line.color;
        options.font_size = 15;
        window.draw_text(line.text, Application::the().fixed_width_font, sf::Vector2f { 5, s * LINE_SPACING + 19 } + scroll_offset(), options);
        s++;
    }

    ScrollableWidget::draw_scrollbar(window);
}

float Console::content_height() const {
    return m_lines.size() * LINE_SPACING + 10;
}

}
