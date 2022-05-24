#include "Tooltip.hpp"

#include "Application.hpp"

#include <EssaGUI/gfx/SFMLWindow.hpp>

namespace GUI {

void TooltipOverlay::draw() {
    TextDrawOptions text;
    text.font_size = 15;
    text.fill_color = sf::Color::Black;
    text.text_align = Align::CenterLeft;

    auto bounds = window().calculate_text_size(m_tooltip.text, Application::the().font, text);

    auto x_pos = std::min(window().getSize().x - (bounds.x + 10), position().x - 5);
    window().draw_rectangle({
        { x_pos, position().y - 10 },
        { bounds.x + 10, bounds.y + 10 },
    });

    // FIXME: Text size is calculated 2x
    window().draw_text_aligned_in_rect(m_tooltip.text, rect(), Application::the().font, text);
}

}
