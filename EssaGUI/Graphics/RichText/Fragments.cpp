#include "Fragments.hpp"
#include "EssaGUI/GUI/Application.hpp"

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

}
