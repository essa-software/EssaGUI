#include "Tooltip.hpp"

#include "Application.hpp"

#include <EssaGUI/gfx/Text.hpp>
#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

void TooltipOverlay::draw(GUI::Window& window) {
    Gfx::Text text { m_tooltip.text, Application::the().font() };
    text.set_font_size(theme().label_font_size);
    text.set_fill_color(theme().tooltip.text);
    auto bounds = text.calculate_text_size();

    RectangleDrawOptions background;
    background.fill_color = theme().tooltip.background;
    background.outline_color = theme().tooltip.foreground;
    background.outline_thickness = -1;

    auto x_pos = std::min<float>(window.size().x() - (bounds.x() + 20), position().x() - 10);
    Util::Rectf bounds_rect {
        { x_pos, position().y() - 15 },
        { bounds.x() + 20, bounds.y() + 12 },
    };

    window.draw_rectangle(bounds_rect, background);

    // FIXME: Text size is calculated 2x
    text.align(Align::Center, bounds_rect);
    text.draw(window);
}

}
