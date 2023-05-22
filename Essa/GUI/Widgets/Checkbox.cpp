#include "Checkbox.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>
#include <array>
#include <cstddef>
#include <span>
#include <vector>

namespace GUI {

void Checkbox::draw(Gfx::Painter& painter) const {
    auto colors = colors_for_state();

    constexpr float checkbox_size = 16;

    Util::Rectf box(2, raw_size().y() / 2 - checkbox_size / 2, checkbox_size, checkbox_size);
    Gfx::RectangleDrawOptions box_opt;
    box_opt.outline_thickness = 1;
    box_opt.outline_color = colors.foreground;
    box_opt.fill_color = colors.background;
    painter.deprecated_draw_rectangle(box, box_opt);

    // TODO: Take advantage of GUIBuilder
    if (is_active()) {
        switch (m_style) {
        case Style::CROSS: {
            std::array<Gfx::Vertex, 2> vertex_arr;
            Gfx::Vertex vert;
            vert.color() = theme().placeholder;

            vert.position() = { box.left + 3, box.top + 3 };
            vertex_arr[0] = vert;

            vert.position() = { box.left + box.width - 3, box.top + box.height - 3 };
            vertex_arr[1] = vert;

            painter.draw_vertices(llgl::PrimitiveType::Lines, vertex_arr);

            vert.position() = { box.left + 3, box.top + box.height - 3 };
            vertex_arr[0] = vert;

            vert.position() = { box.left + box.width - 3, box.top + 3 };
            vertex_arr[1] = vert;

            painter.draw_vertices(llgl::PrimitiveType::Lines, vertex_arr);
            break;
        }
        case Style::MARK: {
            std::array<Gfx::Vertex, 3> vertex_arr;
            Gfx::Vertex vert;
            vert.color() = theme().placeholder;

            vert.position() = { box.left + 3, box.top + box.height / 2 };
            vertex_arr[0] = vert;

            vert.position() = { box.left + box.width / 2 - 2, box.top + box.height - 3 };
            vertex_arr[1] = vert;

            vert.position() = { box.left + box.width - 3, box.top + 3 };
            vertex_arr[2] = vert;

            painter.draw_vertices(llgl::PrimitiveType::LineStrip, vertex_arr);

            break;
        }
        }
    }

    Util::Rectf text_rect(7 + box.width, 0, raw_size().x() - 10, raw_size().y());

    Gfx::Text text { m_caption, Application::the().font() };
    text.set_fill_color(theme().label.text);
    text.set_font_size(theme().label_font_size);
    text.align(GUI::Align::CenterLeft, text_rect);
    text.draw(painter);
}

Theme::ButtonColors Checkbox::default_button_colors() const {
    Theme::ButtonColors colors;

    colors.active.unhovered = theme().textbox.normal;
    colors.active.hovered = theme().textbox.normal;
    colors.inactive.unhovered = theme().textbox.normal;
    colors.inactive.hovered = theme().textbox.normal;
    colors.disabled = theme().textbox.disabled;

    return colors;
}

EML::EMLErrorOr<void> Checkbox::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_caption = TRY(object.get_property("caption", EML::Value(Util::UString {})).to_string());
    auto mark_type = TRY(object.get_property("box_type", EML::Value("cross")).to_string());

    if (mark_type == "cross") {
        m_style = Style::CROSS;
    }
    else if (mark_type == "mark") {
        m_style = Style::MARK;
    }

    return {};
}

EML_REGISTER_CLASS(Checkbox);

}
