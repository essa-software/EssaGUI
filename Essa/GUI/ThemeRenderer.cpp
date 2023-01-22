#include "ThemeRenderer.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Theme.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

namespace GUI {

void DefaultThemeRenderer::draw_text_button_background(GUI::TextButton const& button, Gfx::Painter& painter) const {
    auto colors = button.colors_for_state();

    Gfx::RectangleDrawOptions rect;
    rect.fill_color = colors.background;
    painter.deprecated_draw_rectangle(button.local_rect(), rect);
}

void DefaultThemeRenderer::draw_text_editor_background(GUI::Widget const& widget, Gfx::Painter& painter) const {
    auto theme_colors = theme().textbox.value(widget);

    Gfx::RectangleDrawOptions background_rect;
    background_rect.fill_color = theme_colors.background;
    painter.deprecated_draw_rectangle(widget.local_rect(), background_rect);
}

void DefaultThemeRenderer::draw_text_editor_border(GUI::Widget const& widget, bool multiline, Gfx::Painter& painter) const {
    auto theme_colors = theme().textbox.value(widget);

    Gfx::RectangleDrawOptions border_rect;
    border_rect.fill_color = Util::Colors::Transparent;
    border_rect.outline_color = (widget.is_focused() && !multiline) ? theme().focus_frame : theme_colors.foreground;
    border_rect.outline_thickness = -1;
    painter.deprecated_draw_rectangle(widget.local_rect(), border_rect);
}

}
