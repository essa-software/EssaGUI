#include "Checkbox.hpp"
#include "Application.hpp"
#include "EssaGUI/gfx/Window.hpp"
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/Core/Vertex.hpp>
#include <array>
#include <cstddef>
#include <span>
#include <vector>

namespace GUI{

void Checkbox::draw(GUI::Window &window) const{
    Util::Rectf box(2, 2, size().y() - 4, size().y() - 4);
    RectangleDrawOptions box_opt;
    box_opt.outline_thickness = 1;
    box_opt.outline_color = get_foreground_color();
    box_opt.fill_color = get_background_color();

    window.draw_rectangle(box, box_opt);

    if(is_active()){
        switch (m_style) {
            case Style::CROSS:{
                std::array<llgl::Vertex, 2> vertex_arr;
                llgl::Vertex vert;
                vert.color = get_foreground_color();

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
            break;}
            case Style::MARK:{
                std::array<llgl::Vertex, 3>  vertex_arr;
                llgl::Vertex vert;
                vert.color = get_foreground_color();

                vert.position = Util::Vector3f(box.left + box.width * .2f, box.top + box.height * .4f, 0);
                vertex_arr[0] = vert;

                vert.position = Util::Vector3f(box.left + box.width * .5f, box.top + box.height * .7f, 0);
                vertex_arr[1] = vert;

                vert.position = Util::Vector3f(box.left + box.width * .8f, box.top + box.height * .2f, 0);
                vertex_arr[2] = vert;
                
                window.draw_vertices(llgl::opengl::PrimitiveType::LineStrip, vertex_arr);

            break;}
        }
    }

    Util::Rectf text_rect(position().x() + size().y() + 5, 0, local_rect().left - size().y() - 5, local_rect().height);
    TextDrawOptions text_opt;
    text_opt.text_align = Align::CenterLeft;
    text_opt.fill_color = get_text_color();
    text_opt.font_size = size().y() - 2;

    window.draw_text_aligned_in_rect(m_caption, text_rect, Application::the().font(), text_opt);
}

Theme::ButtonColors Checkbox::default_button_colors() const{
    Theme::ButtonColors colors;
    colors.active.background = Util::Colors::white;
    colors.active.foreground = Util::Colors::black;
    colors.active.text = Util::Colors::black;

    colors.inactive.background = Util::Colors::white;
    colors.inactive.foreground = Util::Colors::black;
    colors.inactive.text = Util::Colors::black;

    colors.untoggleable.background = Util::Colors::white;
    colors.untoggleable.foreground = Util::Color(50, 50, 50);
    colors.untoggleable.text = Util::Color(50, 50, 50);

    return colors;
}

}
