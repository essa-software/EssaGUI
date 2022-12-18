#include "ThemeRenderer.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Theme.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>

namespace GUI {

void DefaultThemeRenderer::draw_text_button_background(GUI::TextButton const& button, Gfx::Painter& painter) const {
    auto colors = button.colors_for_state();

    Gfx::RectangleDrawOptions rect;
    rect.fill_color = colors.background;
    painter.draw_rectangle(button.local_rect(), rect);
}

}
