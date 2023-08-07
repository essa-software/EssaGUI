#include "Textfield.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/RichText.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

#include <Essa/GUI/TextAlign.hpp>

namespace GUI {

Textfield::Textfield()
    : m_font(&GUI::Application::the().font()) { }

Gfx::RichTextContext Textfield::rich_text_context() const {
    return {
        .default_font = *m_font,
        .font_color = theme().label.text,
        .font_size = static_cast<int>(m_font_size),
        .text_alignment = m_alignment,
    };
}

void Textfield::draw(Gfx::Painter& painter) const {
    assert(m_font);

    auto theme_colors = theme().label;

    Gfx::RectangleDrawOptions rect;
    rect.fill_color = theme_colors.background;
    painter.deprecated_draw_rectangle(local_rect().cast<float>(), rect);

    std::visit(
        Util::Overloaded {
            [&](Util::UString const& string) {
                // FIXME: Maybe use RichTextDrawable also here??
                Gfx::Text text { string, *m_font };
                text.set_fill_color(theme_colors.text);
                text.set_font_size(m_font_size);
                text.align(m_alignment, text_rect().cast<float>());
                text.draw(painter);
            },
            [&](Gfx::RichText const& richtext) {
                Gfx::RichTextDrawable drawable(richtext, rich_text_context());
                drawable.set_rect(text_rect().cast<float>());
                drawable.draw(painter);
            },
        },
        m_content
    );
}

Util::Recti Textfield::text_rect() const {
    auto rect = local_rect();
    rect.left += m_padding;
    rect.top += m_padding;
    rect.width -= m_padding * 2;
    rect.height -= m_padding * 2;
    return rect;
}

Util::Size2u Textfield::needed_size_for_text() const {
    return std::visit(
        Util::Overloaded {
            [&](Util::UString const& string) -> Util::Size2u {
                Gfx::Text text { string, Application::the().font() };
                text.set_font_size(m_font_size);
                auto size = text.calculate_text_size();
                return { size.x() + m_padding * 2, size.y() + m_padding * 2 };
            },
            [&](Gfx::RichText const&) -> Util::Size2u {
                // FIXME: Implement measuring rich text size.
                return {};
            },
        },
        m_content
    );
}

LengthVector Textfield::initial_size() const {
    return std::visit(
        Util::Overloaded {
            [&](Util::UString const&) -> LengthVector {
                auto size = needed_size_for_text();
                return LengthVector {
                    { static_cast<int>(size.x()), Util::Length::Px },
                    { static_cast<int>(size.y()), Util::Length::Px },
                };
            },
            [&](Gfx::RichText const&) -> LengthVector {
                // FIXME: Implement measuring rich text size.
                return LengthVector { Util::Length::Auto, Util::Length::Auto };
            },
        },
        m_content
    );
}

EML::EMLErrorOr<void> Textfield::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));
    m_content = TRY(object.get_property("content", EML::Value("")).to_string());
    m_font_size = TRY(object.get_property("font_size", EML::Value(static_cast<double>(m_font_size))).to_double());
    m_alignment = TRY(object.get_enum("alignment", text_align_from_string, Align::CenterLeft));
    // TODO: Alignment
    m_padding = TRY(object.get_property("padding", EML::Value(static_cast<double>(m_padding))).to_double());
    return {};
}

EML_REGISTER_CLASS(Textfield);

}
