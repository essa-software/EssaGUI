#include "RectangleShape.hpp"

namespace GUI {

void RectangleShape::draw(Gfx::Painter& painter) const {
    Gfx::RectangleDrawOptions options;
    options.fill_color = background_color();
    options.outline_color = foreground_color();
    options.outline_thickness = outline_thickness();
    options.border_radius_top_left = m_border_radius[0];
    options.border_radius_top_right = m_border_radius[1];
    options.border_radius_bottom_left = m_border_radius[2];
    options.border_radius_top_right = m_border_radius[3];

    painter.deprecated_draw_rectangle(local_rect().cast<float>(), options);
}

void RectangleShape::set_border_radius(RectangleVertex vert, float value) {
    switch (vert) {
    case RectangleVertex::TOPLEFT:
        m_border_radius[0] = value;
        return;
    case RectangleVertex::TOPRIGHT:
        m_border_radius[1] = value;
        return;
    case RectangleVertex::BOTTOMLEFT:
        m_border_radius[2] = value;
        return;
    case RectangleVertex::BOTTOMRIGHT:
        m_border_radius[3] = value;
        return;
    case RectangleVertex::ALL:
        m_border_radius[0] = value;
        m_border_radius[1] = value;
        m_border_radius[2] = value;
        m_border_radius[3] = value;
        return;
    }
}

EML::EMLErrorOr<void> RectangleShape::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    return {};
}

EML_REGISTER_CLASS(RectangleShape);

}
