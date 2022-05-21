#include "ImageButton.hpp"

namespace GUI {

ImageButton::ImageButton(Container& c, sf::Image img)
    : Button(c) {
    m_texture.loadFromImage(img);
    m_texture.setSmooth(true);
}

void ImageButton::draw(sf::RenderWindow& window) const {
    sf::CircleShape cs_bg(0.5);
    cs_bg.setScale(size());
    cs_bg.setFillColor(bg_color_for_state());
    if(is_focused()) {
        cs_bg.setOutlineColor(sf::Color(200, 200, 200));
        cs_bg.setOutlineThickness(-0.05);
    }
    window.draw(cs_bg);

    sf::Sprite sprite;
    sprite.setTexture(m_texture);

    auto tex_size = sprite.getTexture()->getSize();
    sprite.setScale(size().x / tex_size.x, size().y / tex_size.y);

    window.draw(sprite);
}

LengthVector ImageButton::initial_size() const {
    // TODO: Stop special-casing!
    return { 72.0_px, 72.0_px };
}

}
