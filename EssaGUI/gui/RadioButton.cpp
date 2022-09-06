#include "RadioButton.hpp"
#include "Application.hpp"

#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>

namespace GUI {

void RadioButton::draw(GUI::Window& window) const {
    Util::Vector2f circle_size(size().y(), size().y());
    Util::Vector2f circle_pos = circle_size / 2;

    circle_size -= Util::Vector2f(2, 2);

    auto colors = colors_for_state();

    DrawOptions circle_opt;
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

    Util::Rectf text_rect(position().x() + size().y() + 5, 0, local_rect().left - size().y() - 5, local_rect().height);
    TextDrawOptions text_opt;
    text_opt.text_align = Align::CenterLeft;
    text_opt.fill_color = theme().label.text;
    text_opt.font_size = size().y() - 2;

    window.draw_text_aligned_in_rect(m_caption, text_rect, Application::the().font(), text_opt);
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
