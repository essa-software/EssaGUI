#include "ImageButton.hpp"

#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <array>

namespace GUI {

void ImageButton::draw(Gfx::Painter& painter) const {
    auto colors = colors_for_state();

    Gfx::DrawOptions cs_bg;
    cs_bg.fill_color = colors.background;
    cs_bg.outline_color = colors.foreground;
    cs_bg.outline_thickness = -0.05;
    painter.draw_ellipse(raw_size() / 2.f, raw_size(), cs_bg);

    if (!m_image)
        return;
    Gfx::RectangleDrawOptions sprite;
    sprite.texture = m_image;
    painter.deprecated_draw_rectangle(local_rect(), sprite);
}

LengthVector ImageButton::initial_size() const {
    // FIXME: More convenient way to convert theme values to pixels. Maybe
    //        just support units in themes.
    return {
        { static_cast<float>(theme().image_button_size), Util::Length::Px },
        { static_cast<float>(theme().image_button_size), Util::Length::Px },
    };
}

}
