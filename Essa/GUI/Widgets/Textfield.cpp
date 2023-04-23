#include "Textfield.hpp"
#include "Essa/GUI/Widgets/Widget.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Text.hpp>

#include <Essa/GUI/TextAlign.hpp>

namespace GUI {

Textfield::Textfield()
    : m_font(&GUI::Application::the().font()) { }

void Textfield::draw(Gfx::Painter& window) const {
    assert(m_font);

    auto theme_colors = theme().label;

    Gfx::RectangleDrawOptions rect;
    rect.fill_color = theme_colors.background;
    window.deprecated_draw_rectangle(local_rect().cast<float>(), rect);

    Gfx::Text text { m_content, *m_font };
    text.set_fill_color(theme_colors.text);
    text.set_font_size(m_font_size);
    text.align(m_alignment, text_rect().cast<float>());
    text.draw(window);
}

Util::Recti Textfield::text_rect() const {
    auto rect = local_rect();
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
    return { { (static_cast<int>(size.x()) + m_padding * 2), Util::Length::Px },
        { (static_cast<int>(size.y()) + m_padding * 2), Util::Length::Px } };
}

EML::EMLErrorOr<void> Textfield::load_from_eml_object(
    EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));
    m_content = TRY(object.get_property("content", EML::Value("")).to_string());
    m_font_size = TRY(object
                          .get_property("font_size",
                              EML::Value(static_cast<double>(m_font_size)))
                          .to_double());
    m_alignment = TRY(object.get_enum(
        "alignment", text_align_from_string, Align::CenterLeft));
    // TODO: Alignment
    m_padding = TRY(
        object
            .get_property("padding", EML::Value(static_cast<double>(m_padding)))
            .to_double());
    return {};
}

EML_REGISTER_CLASS(Textfield);

}
