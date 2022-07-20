#include "Textfield.hpp"
#include "Application.hpp"
#include "TextAlign.hpp"
#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

void Textfield::draw(GUI::Window& window) const {
    RectangleDrawOptions rect;
    rect.fill_color = get_background_color();
    window.draw_rectangle(local_rect(), rect);

    TextDrawOptions text;
    text.fill_color = get_text_color();
    text.font_size = m_font_size;
    text.text_align = m_alignment;
    window.draw_text_aligned_in_rect(m_content, text_rect(), Application::the().font(), text);
}

Util::Rectf Textfield::text_rect() const {
    Util::Rectf rect = local_rect();
    rect.left += m_padding;
    rect.top += m_padding;
    rect.width -= m_padding * 2;
    rect.height -= m_padding * 2;
    return rect;
}

}
