#include "Textfield.hpp"
#include "Application.hpp"
#include "TextAlign.hpp"
#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

void Textfield::draw(GUI::Window& window) const {
    auto theme_colors = theme().label;

    RectangleDrawOptions rect;
    rect.fill_color = theme_colors.background;
    window.draw_rectangle(local_rect(), rect);

    TextDrawOptions text;
    text.fill_color = theme_colors.text;
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

LengthVector Textfield::initial_size() const {
    TextDrawOptions text;
    text.font_size = m_font_size;
    text.text_align = m_alignment;
    auto size = window().calculate_text_size(m_content, Application::the().font(), text);
    return { { static_cast<float>(size.x() + m_padding * 2), Length::Px }, { static_cast<float>(size.y() + m_padding * 2), Length::Px } };
}

}
