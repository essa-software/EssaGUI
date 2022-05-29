#include "ImageWidget.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"

namespace GUI {

void ImageWidget::draw(GUI::SFMLWindow& window) const {
    assert(m_image);

    // TODO: Move this logic to SFMLWindow (e.g draw_best_fit_image)
    auto image_size = m_image->getSize();
    float aspect = size().x / size().y;
    float image_aspect = static_cast<float>(image_size.x) / image_size.y;
    sf::Vector2f rect_size;
    if (aspect > image_aspect)
        rect_size = { size().y * image_aspect, size().y };
    else
        rect_size = { size().x, size().x / image_aspect };

    RectangleDrawOptions rect;
    rect.texture = m_image;
    window.draw_rectangle({ size() / 2.f - rect_size / 2.f, rect_size }, rect);
}

}
