#include "ImageButton.hpp"

#include <EssaGUI/gfx/Window.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <array>

namespace GUI {

ImageButton::ImageButton(llgl::opengl::Texture const& image)
    : Button()
    , m_texture(std::move(image)) {
}

void ImageButton::draw(GUI::Window& window) const {
    auto colors = colors_for_state();

    DrawOptions cs_bg;
    cs_bg.fill_color = colors.background;
    cs_bg.outline_color = colors.foreground;
    cs_bg.outline_thickness = -0.05;
    window.draw_ellipse(size() / 2.f, size(), cs_bg);

    // TODO: Implement TextureScope
    RectangleDrawOptions sprite;
    sprite.texture = &m_texture;
    window.draw_rectangle(local_rect(), sprite);
}

LengthVector ImageButton::initial_size() const {
    // TODO: Stop special-casing!
    return { 72.0_px, 72.0_px };
}

}
