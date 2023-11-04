#include "TextButton.hpp"

#include "Button.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/DefaultResources.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/Text.hpp>

#include <Essa/GUI/ThemeRenderer.hpp>

namespace GUI {

void TextButton::draw(Gfx::Painter& painter) const {
    auto colors = colors_for_state();

    theme().renderer().draw_text_button_background(*this, painter);

    double text_offset = 0;
    if (m_image) {
        Gfx::RectangleDrawOptions image;
        image.texture = m_image;
        if (m_content.is_empty()) {
            painter.deprecated_draw_rectangle(
                {
                    (raw_size() / 2 - m_image->size().cast<int>() / 2).cast<float>().to_vector().to_point(),
                    m_image->size().cast<float>(),
                },
                image
            );
            return;
        }
        else {
            text_offset = 5 + m_image->size().x();
            painter.deprecated_draw_rectangle(
                { { 5, raw_size().y() / 2 - m_image->size().y() / 2.f },
                  { static_cast<float>(m_image->size().x()), static_cast<float>(m_image->size().y()) } },
                image
            );
        }
    }

    Gfx::Text text { "", Application::the().font() };
    text.set_font_size(theme().label_font_size);
    text.set_fill_color(colors.text);

    auto text_rect = local_rect();
    text_rect.left += text_offset;
    text_rect.width -= text_offset;

    if (is_active())
        text.set_string(m_active_content);
    else
        text.set_string(m_content);

    text.align(GUI::Align::Center, text_rect.cast<float>());
    text.draw(painter);

    if (is_focused()) {
        Gfx::RectangleDrawOptions focus_rect;
        focus_rect.fill_color = Util::Colors::Transparent;
        focus_rect.outline_color = theme().focus_frame;
        focus_rect.outline_thickness = -1;
        painter.deprecated_draw_rectangle(local_rect().cast<float>(), focus_rect);
    }
}

EML::EMLErrorOr<void> TextButton::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Button::load_from_eml_object(object, loader));
    m_content = TRY(object.get_property("content", EML::Value("")).to_string());
    m_active_content = TRY(object.get_property("active_content", EML::Value("")).to_string());
    auto image = object.get_property("image", EML::Value(false)).to_resource_id();
    if (!image.is_error())
        m_image = resource_manager().get<Gfx::Texture>(image.release_value());
    return {};
}

LengthVector TextButton::initial_size() const {
    float width = GUI::Application::the().font().calculate_text_size(m_content, theme().label_font_size).x() + 20;
    if (m_image) {
        width += m_image->size().x() + 5;
    }
    return { { width, Util::Length::Px }, { static_cast<float>(theme().line_height), Util::Length::Px } };
}

EML_REGISTER_CLASS(TextButton);

}
