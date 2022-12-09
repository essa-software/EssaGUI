#include "TextButton.hpp"

#include <Essa/GUI/Application.hpp>
#include "Button.hpp"
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Graphics/Window.hpp>

namespace GUI {

void TextButton::draw(Gfx::Painter& window) const {
    auto colors = colors_for_state();

    Gfx::RectangleDrawOptions rect;
    rect.fill_color = colors.background;
    window.draw_rectangle(local_rect(), rect);

    double text_offset = 0;
    if (m_image) {
        Gfx::RectangleDrawOptions image;
        image.texture = m_image;
        if (m_content.is_empty()) {
            window.draw_rectangle({ raw_size() / 2.f - Util::Vector2f { m_image->size() } / 2.f,
                                      Util::Vector2f { m_image->size() } },
                image);
            return;
        }
        else {
            text_offset = 5 + m_image->size().x();
            window.draw_rectangle(
                { { 5, raw_size().y() / 2 - m_image->size().y() / 2.f },
                    { static_cast<float>(m_image->size().x()), static_cast<float>(m_image->size().y()) } },
                image);
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

    text.align(GUI::Align::Center, text_rect);
    text.draw(window);
}

EML::EMLErrorOr<void> TextButton::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Button::load_from_eml_object(object, loader));
    m_content = TRY(object.get_property("content", Util::UString {}).to_string());
    m_active_content = TRY(object.get_property("active_content", Util::UString {}).to_string());
    auto image = object.get_property("image", false).to_resource_id();
    if (!image.is_error())
        m_image = resource_manager().get<Gfx::Texture>(image.release_value());
    return {};
}

EML_REGISTER_CLASS(TextButton);

}
