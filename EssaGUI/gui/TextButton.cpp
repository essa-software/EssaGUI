#include "TextButton.hpp"

#include "Application.hpp"
#include "Button.hpp"
#include <EssaGUI/eml/Loader.hpp>
#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

void TextButton::draw(GUI::Window& window) const {
    auto colors = colors_for_state();

    RectangleDrawOptions rect;
    rect.fill_color = colors.background;
    window.draw_rectangle(local_rect(), rect);

    double text_offset = 0;
    if (m_image) {
        RectangleDrawOptions image;
        image.texture = m_image;
        if (m_content.is_empty()) {
            window.draw_rectangle({ size() / 2.f - Util::Vector2f { m_image->size() } / 2.f,
                                      Util::Vector2f { m_image->size() } },
                image);
            return;
        }
        else {
            text_offset = 5 + m_image->size().x();
            window.draw_rectangle(
                { { 5, size().y() / 2 - m_image->size().y() / 2.f },
                    { static_cast<float>(m_image->size().x()), static_cast<float>(m_image->size().y()) } },
                image);
        }
    }

    TextDrawOptions text;
    text.font_size = theme().label_font_size;
    text.fill_color = colors.text;
    text.text_align = m_alignment;

    auto text_rect = local_rect();
    text_rect.left += text_offset;
    text_rect.width -= text_offset;

    if (is_active())
        window.draw_text_aligned_in_rect(m_active_content, local_rect(), Application::the().font(), text);
    else
        window.draw_text_aligned_in_rect(m_content, local_rect(), Application::the().font(), text);
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
