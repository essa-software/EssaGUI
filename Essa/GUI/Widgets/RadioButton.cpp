#include "RadioButton.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/AST.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Drawing/Ellipse.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>

namespace GUI {

constexpr float circle_radius = 8;
constexpr float text_circle_padding = 5;
constexpr float text_offset = circle_radius * 2 + text_circle_padding + 2;

void RadioButton::draw(Gfx::Painter& window) const {
    auto colors = colors_for_state();

    window.draw(Gfx::Drawing::Ellipse { { 2 + circle_radius, raw_size().y() / 2 },
                                        circle_radius,
                                        Gfx::Drawing::Fill::solid(colors.background),
                                        Gfx::Drawing::Outline::normal(colors.foreground, 1) });

    if (is_active()) {
        constexpr float active_circle_radius = 4;

        window.draw(Gfx::Drawing::Ellipse {
            { 2 + circle_radius, raw_size().y() / 2 }, active_circle_radius, Gfx::Drawing::Fill::solid(theme().placeholder) });
    }

    Util::Rectf text_rect = local_rect().cast<float>().expand_left(-text_offset);
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

    m_caption = TRY(object.get_property("caption", EML::Value(Util::UString {})).to_string());

    return {};
}

LengthVector RadioButton::initial_size() const {
    return {
        { Application::the().font().calculate_text_size(m_caption, theme().label_font_size).x() + text_offset, Util::Length::Px },
        { static_cast<float>(theme().line_height), Util::Length::Px },
    };
}

EML_REGISTER_CLASS(RadioButton);
}
