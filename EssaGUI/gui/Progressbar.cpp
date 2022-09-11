#include "Progressbar.hpp"
#include "EssaGUI/gfx/Window.hpp"
#include "EssaGUI/gui/Application.hpp"
#include "EssaGUI/gui/TextAlign.hpp"
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <cstddef>

namespace GUI {

Util::UString Progressbar::get_content_with_labelling() const {
    switch (m_labelling) {
    case Labelling::NONE:
        return m_content;
    case Labelling::PERCENTAGE:
        return m_content + " (" + Util::to_ustring((int)(((float)m_value / (float)m_max) * 100)) + "%)";
    case Labelling::VALUE:
        return m_content + " (" + Util::to_ustring(m_value) + " / " + Util::to_ustring(m_max) + ")";
    }
    return m_content;
}

void Progressbar::draw(GUI::Window& window) const {
    auto theme_colors = theme().textbox.value(*this);

    RectangleDrawOptions background_rect;
    background_rect.fill_color = theme_colors.background;
    background_rect.outline_color = is_focused() ? theme().focus_frame : theme_colors.foreground;
    background_rect.outline_thickness = -1;
    window.draw_rectangle(local_rect(), background_rect);

    Util::Rectf completed_rect = local_rect();
    completed_rect.width = std::min((float)m_value / (float)m_max * raw_size().x(), raw_size().x() - 4);
    completed_rect.height -= 4;
    completed_rect.left = 2;
    completed_rect.top = 2;
    RectangleDrawOptions completed_opt;
    completed_opt.fill_color = m_progressbar_color;
    window.draw_rectangle(completed_rect, completed_opt);

    TextDrawOptions text_opt;
    text_opt.text_align = Align::Center;
    text_opt.font_size = raw_size().y() - 2;
    text_opt.fill_color = theme_colors.text;
    window.draw_text_aligned_in_rect(get_content_with_labelling(), local_rect(), Application::the().font(), text_opt);
}

void Progressbar::step_by() {
    m_value += m_step;

    if (m_value >= m_max) {
        m_value = m_max;

        if (on_finish)
            on_finish();
    }
}

}
