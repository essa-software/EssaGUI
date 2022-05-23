#include "TextButton.hpp"

#include "Application.hpp"
#include "Button.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"

namespace GUI {

TextButton::TextButton(Container& c)
    : Button(c) {
}

void TextButton::draw(GUI::SFMLWindow& window) const {
    RectangleDrawOptions rect;
    rect.fill_color = bg_color_for_state();
    window.draw_rectangle(local_rect(), rect);

    TextDrawOptions text;
    text.font_size = 15;
    text.fill_color = text_color_for_state();
    text.text_align = m_alignment;

    if (is_active())
        window.draw_text_aligned_in_rect(m_active_content, local_rect(), Application::the().font, text);
    else
        window.draw_text_aligned_in_rect(m_content, local_rect(), Application::the().font, text);
}

}
