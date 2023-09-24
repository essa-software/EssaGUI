#include "ImageWidget.hpp"

#include <Essa/GUI/Graphics/ResourceManager.hpp>

namespace GUI {

void ImageWidget::draw(Gfx::Painter& painter) const {
    if (!m_image)
        return;
    // TODO: Move this logic to Painter (e.g draw_best_fit_image)
    auto image_size = m_image->size();
    if (raw_size().y() == 0) {
        return;
    }
    float aspect = static_cast<float>(raw_size().x()) / static_cast<float>(raw_size().y());
    float image_aspect = static_cast<float>(image_size.x()) / static_cast<float>(image_size.y());
    Util::Size2i rect_size;
    if (aspect > image_aspect)
        rect_size = { static_cast<float>(raw_size().y()) * image_aspect, raw_size().y() };
    else
        rect_size = { raw_size().x(), static_cast<float>(raw_size().x()) / image_aspect };

    Gfx::RectangleDrawOptions rect;
    rect.texture = m_image;
    painter.deprecated_draw_rectangle(
        { (raw_size() / 2.f - rect_size.to_vector() / 2.f).cast<float>().to_vector().to_point(), rect_size.cast<float>() }, rect
    );
}

EML::EMLErrorOr<void> ImageWidget::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_image = resource_manager().get<Gfx::Texture>(TRY(TRY(object.require_property("image")).to_resource_id()));

    return {};
}

EML_REGISTER_CLASS(ImageWidget)

}
