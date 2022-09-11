#include "Checkbox.hpp"
#include "Application.hpp"
#include "EssaGUI/gfx/Window.hpp"
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/Core/Vertex.hpp>
#include <array>
#include <cstddef>
#include <span>
#include <vector>

namespace GUI {

void Checkbox::draw(GUI::Window& window) const {
    auto colors = colors_for_state();

    Util::Rectf box(2, 2, raw_size().y() - 4, raw_size().y() - 4);
    RectangleDrawOptions box_opt;
    box_opt.outline_thickness = 1;
    box_opt.outline_color = colors.foreground;
    box_opt.fill_color = colors.background;

    window.draw_rectangle(box, box_opt);

    if (is_active()) {
        switch (m_style) {
        case Style::CROSS: {
            std::array<llgl::Vertex, 2> vertex_arr;
            llgl::Vertex vert;
            vert.color = theme().placeholder;

            vert.position = Util::Vector3f(box.left + box.width * .2f, box.top + box.height * .2f, 0);
            vertex_arr[0] = vert;

            vert.position = Util::Vector3f(box.left + box.width * .8f, box.top + box.height * .8f, 0);
            vertex_arr[1] = vert;

            window.draw_vertices(llgl::opengl::PrimitiveType::Lines, vertex_arr);

            vert.position = Util::Vector3f(box.left + box.width * .2f, box.top + box.height * .8f, 0);
            vertex_arr[0] = vert;

            vert.position = Util::Vector3f(box.left + box.width * .8f, box.top + box.height * .2f, 0);
            vertex_arr[1] = vert;

            window.draw_vertices(llgl::opengl::PrimitiveType::Lines, vertex_arr);
            break;
        }
        case Style::MARK: {
            std::array<llgl::Vertex, 3> vertex_arr;
            llgl::Vertex vert;
            vert.color = theme().placeholder;

            vert.position = Util::Vector3f(box.left + box.width * .2f, box.top + box.height * .4f, 0);
            vertex_arr[0] = vert;

            vert.position = Util::Vector3f(box.left + box.width * .5f, box.top + box.height * .7f, 0);
            vertex_arr[1] = vert;

            vert.position = Util::Vector3f(box.left + box.width * .8f, box.top + box.height * .2f, 0);
            vertex_arr[2] = vert;

            window.draw_vertices(llgl::opengl::PrimitiveType::LineStrip, vertex_arr);

            break;
        }
        }
    }

    Util::Rectf text_rect(raw_position().x() + raw_size().y() + 5, 0, local_rect().left - raw_size().y() - 5, local_rect().height);
    TextDrawOptions text_opt;
    text_opt.text_align = Align::CenterLeft;
    text_opt.fill_color = theme().label.text;
    text_opt.font_size = theme().label_font_size;

    window.draw_text_aligned_in_rect(m_caption, text_rect, Application::the().font(), text_opt);
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
    
    m_caption = TRY(object.get_property("caption", Util::UString{}).to_string());
    auto mark_type = TRY(object.get_property("box_type", Util::UString("cross")).to_string());

    if(mark_type == "cross"){
        m_style = Style::CROSS;
    }else if(mark_type == "mark"){
        m_style = Style::MARK;
    }

    return {};
}

EML_REGISTER_CLASS(Checkbox);

}
