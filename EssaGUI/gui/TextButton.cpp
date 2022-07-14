#include "TextButton.hpp"

#include "Application.hpp"
#include "Button.hpp"
#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

TextButton::TextButton(Container& c)
    : Button(c) {
}

void TextButton::draw(GUI::Window& window) const {
    RectangleDrawOptions rect;
    rect.fill_color = bg_color_for_state();
    window.draw_rectangle(local_rect(), rect);

    double text_offset = 0;
    if (m_image) {
        RectangleDrawOptions image;
        image.texture = m_image;
        if (m_content.is_empty()) {
            window.draw_rectangle({ size() / 2.f - Util::Vector2f { m_image->size() } / 2.f,
                                      Util::Vector2f { m_image->size() } },
                image);
            return;
        }
        else {
            text_offset = 5 + m_image->size().x();
            window.draw_rectangle(
                { { 5, size().y() / 2 - m_image->size().y() / 2.f },
                    { static_cast<float>(m_image->size().x()), static_cast<float>(m_image->size().y()) } },
                image);
        }
    }

    TextDrawOptions text;
    text.font_size = 15;
    text.fill_color = text_color_for_state();
    text.text_align = m_alignment;

    auto text_rect = local_rect();
    text_rect.left += text_offset;
    text_rect.width -= text_offset;

    if (is_active())
        window.draw_text_aligned_in_rect(m_active_content, local_rect(), Application::the().font, text);
    else
        window.draw_text_aligned_in_rect(m_content, local_rect(), Application::the().font, text);
}

}
