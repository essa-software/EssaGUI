#include "Fragments.hpp"

#include <Essa/GUI/Application.hpp>

namespace Gfx::RichTextFragments {

float Text::wanted_size(RichTextContext const& context) const {
    return text(context).calculate_text_size().x();
}

void Text::draw(RichTextContext const& context, Util::Vector2f position, Gfx::Painter& painter) const {
    auto text = this->text(context);
    text.set_position(position);
    text.draw(painter);
}

Gfx::Text Text::text(RichTextContext const& context) const {
    Gfx::Text text { m_string, context.default_font };
    text.set_string(m_string);
    text.set_fill_color(m_color);
    return text;
}

float Image::wanted_size(RichTextContext const&) const {
    return m_texture.size().x() + 6;
}

void Image::draw(RichTextContext const&, Util::Vector2f position, Gfx::Painter& painter) const {
    Gfx::RectangleDrawOptions rect;
    rect.texture = &m_texture;
    Util::Vector2f size { m_texture.size() };
    painter.draw_rectangle({ { position.x() + 3, position.y() - size.y() }, size }, rect);
}

}
