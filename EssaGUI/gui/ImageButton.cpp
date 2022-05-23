#include "ImageButton.hpp"
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
    window.set_texture(&m_texture);
    std::array<Vertex, 4> sprite;
    sprite[0] = Vertex { .position = { 0, 0 }, .color = sf::Color::White, .tex_coords = { 0, 0 } };
    sprite[1] = Vertex { .position = { size().x, 0 }, .color = sf::Color::White, .tex_coords = { 1, 0 } };
    sprite[2] = Vertex { .position = { 0, size().y }, .color = sf::Color::White, .tex_coords = { 0, 1 } };
    sprite[3] = Vertex { .position = { size().x, size().y }, .color = sf::Color::White, .tex_coords = { 1, 1 } };
    window.draw_vertices(GL_TRIANGLE_STRIP, sprite);
    window.set_texture(nullptr);
}

LengthVector ImageButton::initial_size() const {
    // TODO: Stop special-casing!
    return { 72.0_px, 72.0_px };
}

}
