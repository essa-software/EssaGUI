#include "Textfield.hpp"
#include "Application.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"
#include "TextAlign.hpp"
#include <SFML/System/Vector2.hpp>

namespace GUI {

void Textfield::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Textfield::draw(GUI::SFMLWindow& window) const {
    RectangleDrawOptions rect;
    rect.fill_color = m_bg_color;
    window.draw_rectangle(local_rect(), rect);

    TextDrawOptions text;
    text.fill_color = m_text_color;
    text.font_size = m_font_size;
    text.text_align = m_alignment;
    window.draw_text_aligned_in_rect(m_content, local_rect(), Application::the().font, text);
}

sf::Vector2f Textfield::calculate_text_size() const {
    // FIXME: Kept for backwards compatibility with ESSA.
    return window().calculate_text_size(m_content, Application::the().font, TextDrawOptions { .font_size = m_font_size });
}

}
