#include "Textfield.hpp"
#include "Application.hpp"
#include <EssaGUI/gfx/Window.hpp>
#include "TextAlign.hpp"

namespace GUI {

void Textfield::draw(GUI::Window& window) const {
    RectangleDrawOptions rect;
    rect.fill_color = get_background_color();
    window.draw_rectangle(local_rect(), rect);

    TextDrawOptions text;
    text.fill_color = get_text_color();
    text.font_size = m_font_size;
    text.text_align = m_alignment;
    window.draw_text_aligned_in_rect(m_content, local_rect(), Application::the().font, text);
}

}
