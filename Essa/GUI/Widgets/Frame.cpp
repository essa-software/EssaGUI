#include "Frame.hpp"

#include <iostream>

namespace GUI {

void Frame::draw(Gfx::Painter& painter) const {
    Gfx::RectangleDrawOptions rect;
    rect.set_border_radius(BorderRadius);
    rect.fill_color = Util::Color(50, 50, 50, 150);
    painter.deprecated_draw_rectangle(local_rect().cast<float>(), rect);
}

}
