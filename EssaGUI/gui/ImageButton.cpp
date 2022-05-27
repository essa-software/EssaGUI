#include "ImageButton.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <array>

namespace GUI {

ImageButton::ImageButton(Container& c, sf::Image img)
    : Button(c) {
    m_texture.loadFromImage(img);
    m_texture.setSmooth(true);
}

void ImageButton::draw(GUI::SFMLWindow& window) const {
    DrawOptions cs_bg;
    cs_bg.fill_color = bg_color_for_state();
    if (is_focused()) {
        cs_bg.outline_color = sf::Color(200, 200, 200);
        cs_bg.outline_thickness = -0.05;
    }
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
