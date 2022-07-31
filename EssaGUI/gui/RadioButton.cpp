#include "RadioButton.hpp"
#include "EssaGUI/gfx/Window.hpp"
#include "Application.hpp"
#include "EssaGUI/gui/TextAlign.hpp"
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>

namespace GUI{

void RadioButton::draw(GUI::Window &window)const{
    Util::Vector2f circle_size(size().y(), size().y());
    Util::Vector2f circle_pos = circle_size / 2;

    circle_size -= Util::Vector2f(2, 2);

    DrawOptions circle_opt;
    circle_opt.outline_color = get_background_color();
    circle_opt.outline_thickness = 2; 
    circle_opt.fill_color = Util::Color(0xffffffff);

    window.draw_ellipse(circle_pos, circle_size, circle_opt);

    if(is_active()){
        circle_size.x() -= 4;
        circle_size.y() -= 4;

        circle_opt.outline_thickness = 0;
        circle_opt.fill_color = Util::Color(50, 50, 50);

        window.draw_ellipse(circle_pos, circle_size, circle_opt);
    }

    Util::Rectf text_rect(position().x() + size().y() + 5, 0, local_rect().left - size().y() - 5, local_rect().height - 2);
    TextDrawOptions text_opt;
    text_opt.text_align = Align::CenterLeft;
    text_opt.fill_color = get_text_color();
    text_opt.font_size = size().y() - 2;

    window.draw_text_aligned_in_rect(m_caption, text_rect, Application::the().font(), text_opt);
}

Theme::ButtonColors RadioButton::default_button_colors() const{
    Theme::ButtonColors colors;
    colors.active.background = Util::Colors::black;
    colors.active.foreground = Util::Color(100, 100, 100);
    colors.active.text = Util::Color(50, 50, 50);

    colors.inactive.background = Util::Colors::black;
    colors.inactive.foreground = Util::Color(100, 100, 100);
    colors.inactive.text = Util::Color(50, 50, 50);

    colors.untoggleable.background = Util::Color(50, 50, 50);
    colors.untoggleable.foreground = Util::Color(150, 150, 150);
    colors.untoggleable.text = Util::Color(100, 100, 100);

    return colors;
}

}
