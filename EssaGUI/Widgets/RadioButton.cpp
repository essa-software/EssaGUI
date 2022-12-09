#include "RadioButton.hpp"

#include <EssaGUI/Application.hpp>
#include <EssaGUI/Graphics/Text.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>

namespace GUI {

void RadioButton::draw(Gfx::Painter& window) const {
    Util::Vector2f circle_size(raw_size().y(), raw_size().y());
    Util::Vector2f circle_pos = circle_size / 2;

    circle_size -= Util::Vector2f(2, 2);

    auto colors = colors_for_state();

    Gfx::DrawOptions circle_opt;
    circle_opt.outline_color = colors.foreground;
    circle_opt.outline_thickness = 2;
    circle_opt.fill_color = colors.background;

    window.draw_ellipse(circle_pos, circle_size, circle_opt);

    if (is_active()) {
        circle_size.x() -= 6;
        circle_size.y() -= 6;

        circle_opt.outline_thickness = 0;
        circle_opt.fill_color = theme().placeholder;

        window.draw_ellipse(circle_pos, circle_size, circle_opt);
    }

    Util::Rectf text_rect(raw_size().y() + 5, 0, local_rect().left - raw_size().y() - 5, local_rect().height);
    Gfx::Text text_opt { m_caption, Application::the().font() };
    text_opt.set_fill_color(theme().label.text);
    text_opt.set_font_size(theme().label_font_size);
    text_opt.align(Align::CenterLeft, text_rect);
    text_opt.draw(window);
}

Theme::ButtonColors RadioButton::default_button_colors() const {
    Theme::ButtonColors colors;

    colors.active.unhovered = theme().textbox.normal;
    colors.active.hovered = theme().textbox.normal;
    colors.inactive.unhovered = theme().textbox.normal;
    colors.inactive.hovered = theme().textbox.normal;
    colors.disabled = theme().textbox.disabled;

    return colors;
}

EML::EMLErrorOr<void> RadioButton::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_caption = TRY(object.get_property("caption", Util::UString {}).to_string());

    return {};
}

EML_REGISTER_CLASS(RadioButton);

}
