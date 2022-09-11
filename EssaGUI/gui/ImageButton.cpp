#include "ImageButton.hpp"

#include <EssaGUI/gfx/Window.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <array>

namespace GUI {

void ImageButton::draw(GUI::Window& window) const {
    auto colors = colors_for_state();

    DrawOptions cs_bg;
    cs_bg.fill_color = colors.background;
    cs_bg.outline_color = colors.foreground;
    cs_bg.outline_thickness = -0.05;
    window.draw_ellipse(raw_size() / 2.f, raw_size(), cs_bg);

    if (!m_image)
        return;
    RectangleDrawOptions sprite;
    sprite.texture = m_image;
    window.draw_rectangle(local_rect(), sprite);
}

LengthVector ImageButton::initial_size() const {
    // TODO: Stop special-casing!
    return { 72.0_px, 72.0_px };
}

}
