#include "ImageButton.hpp"

#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <array>

namespace GUI {

void ImageButton::draw(Gfx::Painter& painter) const {
    auto colors = colors_for_state();

    Gfx::DrawOptions cs_bg;
    cs_bg.fill_color = colors.background;
    cs_bg.outline_color
        = is_focused() ? theme().focus_frame : Util::Colors::Transparent;
    cs_bg.outline_thickness = -1;
    painter.draw_ellipse(raw_size().cast<float>().to_deprecated_vector() / 2.f,
        raw_size().cast<float>().to_deprecated_vector(), cs_bg);

    if (!m_image)
        return;
    Gfx::RectangleDrawOptions sprite;
    sprite.texture = m_image;
    painter.deprecated_draw_rectangle(local_rect().cast<float>(), sprite);
}

LengthVector ImageButton::initial_size() const {
    // FIXME: More convenient way to convert theme values to pixels. Maybe
    //        just support units in themes.
    return {
        { static_cast<int>(theme().image_button_size), Util::Length::Px },
        { static_cast<int>(theme().image_button_size), Util::Length::Px },
    };
}

}
