#include "Tooltip.hpp"

#include "Application.hpp"

#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

void TooltipOverlay::draw() {
    TextDrawOptions text;
    text.font_size = 15;
    text.fill_color = theme().tooltip.text;
    text.text_align = Align::Center;

    auto bounds = window().calculate_text_size(m_tooltip.text, Application::the().font(), text);

    RectangleDrawOptions background;
    background.fill_color = theme().tooltip.background;
    background.outline_color = theme().tooltip.foreground;
    background.outline_thickness = -1;

    auto x_pos = std::min<float>(window().size().x() - (bounds.x() + 20), position().x() - 10);
    Util::Rectf bounds_rect {
        { x_pos, position().y() - 15 },
        { bounds.x() + 20, bounds.y() + 12 },
    };

    window().draw_rectangle(bounds_rect, background);

    // FIXME: Text size is calculated 2x
    window().draw_text_aligned_in_rect(m_tooltip.text, bounds_rect, Application::the().font(), text);
}

}
