#include "Frame.hpp"
#include <EssaGUI/gfx/Window.hpp>

#include <iostream>

namespace GUI {

void Frame::draw(GUI::Window& window) const {
    RectangleDrawOptions rect;
    rect.set_border_radius(BorderRadius);
    rect.fill_color = Util::Color(50, 50, 50, 150);
    window.draw_rectangle(local_rect(), rect);
}

}
