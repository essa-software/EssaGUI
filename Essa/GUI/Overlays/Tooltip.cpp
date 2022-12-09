#include <Essa/GUI/Overlays/Tooltip.hpp>

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Graphics/Window.hpp>

namespace GUI {

void TooltipOverlay::draw(Gfx::Painter& painter) {
    Gfx::Text text { m_tooltip.text, Application::the().font() };
    text.set_font_size(theme().label_font_size);
    text.set_fill_color(theme().tooltip.text);
    auto bounds = text.calculate_text_size();

    Gfx::RectangleDrawOptions background;
    background.fill_color = theme().tooltip.background;
    background.outline_color = theme().tooltip.foreground;
    background.outline_thickness = -1;

    auto x_pos = std::min<float>(host_window().size().x() - (bounds.x() + 20), position().x() - 10);
    Util::Rectf bounds_rect {
        { x_pos, position().y() - 15 },
        { bounds.x() + 20, bounds.y() + 12 },
    };

    painter.draw_rectangle(bounds_rect, background);

    // FIXME: Text size is calculated 2x
    text.align(Align::Center, bounds_rect);
    text.draw(painter);
}

}
