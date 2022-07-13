#include "Textfield.hpp"
#include "Application.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"
#include "TextAlign.hpp"
#include <SFML/System/Vector2.hpp>

namespace GUI {

void Textfield::draw(GUI::SFMLWindow& window) const {
    RectangleDrawOptions rect;
    rect.fill_color = get_background_color();
    window.draw_rectangle(local_rect(), rect);

    TextDrawOptions text;
    text.fill_color = get_text_color();
    text.font_size = m_font_size;
    text.text_align = m_alignment;
    window.draw_text_aligned_in_rect(m_content, local_rect(), Application::the().font, text);
}

Util::Vector2f Textfield::calculate_text_size() const {
    // FIXME: Kept for backwards compatibility with ESSA.
    return window().calculate_text_size(m_content, Application::the().font, TextDrawOptions { .font_size = m_font_size });
}

}
