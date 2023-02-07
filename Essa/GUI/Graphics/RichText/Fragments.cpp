#include "Fragments.hpp"

#include <Essa/GUI/Application.hpp>

namespace Gfx::RichTextFragments {

float Text::wanted_size(RichTextContext const& context) const {
    return text(context).calculate_text_size().x();
}

void Text::draw(RichTextContext const& context, Util::Vector2f position, Gfx::Painter& painter) const {
    auto text = this->text(context);
    text.align(GUI::Align::CenterLeft, { Util::Cs::Point2f::from_deprecated_vector(position), { 0, context.default_font.line_height(context.font_size) } });
    text.draw(painter);
}

Gfx::Text Text::text(RichTextContext const& context) const {
    Gfx::Text text { m_string, context.default_font };
    text.set_string(m_string);
    text.set_fill_color(m_color);
    text.set_font_size(context.font_size);
    return text;
}

float Image::wanted_size(RichTextContext const& context) const {
    auto size = scaled_image_size(context);
    return size.x() + 6;
}

Util::Vector2f Image::scaled_image_size(RichTextContext const& context) const {
    Util::Vector2f size { m_texture.size() };
    if (size.y() > context.default_font.line_height(context.font_size)) {
        size *= context.default_font.line_height(context.font_size) / size.y();
    }
    return size;
};

void Image::draw(RichTextContext const& context, Util::Vector2f position, Gfx::Painter& painter) const {
    Gfx::RectangleDrawOptions rect;
    rect.texture = &m_texture;
    auto size = Util::Cs::Size2f::from_deprecated_vector(scaled_image_size(context));
    auto height = context.default_font.line_height(context.font_size);
    painter.deprecated_draw_rectangle({ { position.x() + 3, position.y() + height / 2.f - size.y() / 2.f }, size }, rect);
}
}
