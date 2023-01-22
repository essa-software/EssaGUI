#include "Textfield.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Graphics/Window.hpp>
#include <Essa/GUI/TextAlign.hpp>

namespace GUI {

Textfield::Textfield()
    : m_font(&GUI::Application::the().font()) {
}

void Textfield::draw(Gfx::Painter& window) const {
    assert(m_font);

    auto theme_colors = theme().label;

    Gfx::RectangleDrawOptions rect;
    rect.fill_color = theme_colors.background;
    window.deprecated_draw_rectangle(local_rect(), rect);

    Gfx::Text text { m_content, *m_font };
    text.set_fill_color(theme_colors.text);
    text.set_font_size(m_font_size);
    text.align(m_alignment, text_rect());
    text.draw(window);
}

Util::Rectf Textfield::text_rect() const {
    Util::Rectf rect = local_rect();
    rect.left += m_padding;
    rect.top += m_padding;
    rect.width -= m_padding * 2;
    rect.height -= m_padding * 2;
    return rect;
}

LengthVector Textfield::initial_size() const {
    Gfx::Text text { m_content, Application::the().font() };
    text.set_font_size(m_font_size);
    auto size = text.calculate_text_size();
    return { { static_cast<float>(size.x() + m_padding * 2), Util::Length::Px }, { static_cast<float>(size.y() + m_padding * 2), Util::Length::Px } };
}

EML::EMLErrorOr<void> Textfield::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));
    m_content = TRY(object.get_property("content", Util::UString { "" }).to_string());
    m_font_size = TRY(object.get_property("font_size", static_cast<double>(m_font_size)).to_double());
    // TODO: Alignment
    m_padding = TRY(object.get_property("padding", static_cast<double>(m_padding)).to_double());
    return {};
}

EML_REGISTER_CLASS(Textfield);

}
