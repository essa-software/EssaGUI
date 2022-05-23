#include "ImageButton.hpp"

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

    // TODO: Support textures
    // sf::Sprite sprite;
    // sprite.setTexture(m_texture);

    // auto tex_size = sprite.getTexture()->getSize();
    // sprite.setScale(size().x / tex_size.x, size().y / tex_size.y);

    // window.draw(sprite);
}

LengthVector ImageButton::initial_size() const {
    // TODO: Stop special-casing!
    return { 72.0_px, 72.0_px };
}

}
