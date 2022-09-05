#include "ImageWidget.hpp"

#include <EssaGUI/gfx/ResourceManager.hpp>
#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

void ImageWidget::draw(GUI::Window& window) const {
    if (!m_image)
        return;
    // TODO: Move this logic to SFMLWindow (e.g draw_best_fit_image)
    auto image_size = m_image->size();
    float aspect = size().x() / size().y();
    float image_aspect = static_cast<float>(image_size.x()) / image_size.y();
    Util::Vector2f rect_size;
    if (aspect > image_aspect)
        rect_size = { size().y() * image_aspect, size().y() };
    else
        rect_size = { size().x(), size().x() / image_aspect };

    RectangleDrawOptions rect;
    rect.texture = m_image;
    window.draw_rectangle({ size() / 2.f - rect_size / 2.f, rect_size }, rect);
}

EML::EMLErrorOr<void> ImageWidget::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_image = resource_manager().get<Gfx::Texture>(TRY(TRY(object.require_property("image")).to_resource_id()));

    return {};
}

EML_REGISTER_CLASS(ImageWidget)

}
