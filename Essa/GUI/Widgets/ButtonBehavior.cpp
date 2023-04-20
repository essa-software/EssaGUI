#include "ButtonBehavior.hpp"

namespace GUI {

Widget::EventHandlerResult ButtonBehavior::on_mouse_move(Event::MouseMove const& event) {
    m_hovered = m_hit_tester(event.local_position());
    return Widget::EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult ButtonBehavior::on_mouse_button_press(Event::MouseButtonPress const& event) {
    m_hovered = m_hit_tester(event.local_position());
    if (event.button() == llgl::MouseButton::Left && m_hovered) {
        m_pressed_on = true;
        m_hovered = true;
        return Widget::EventHandlerResult::Accepted;
    }
    return Widget::EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult ButtonBehavior::on_mouse_button_release(Event::MouseButtonRelease const& event) {
    if (event.button() == llgl::MouseButton::Left && m_hovered && m_pressed_on) {
        m_pressed_on = false;
        on_click();
    }
    return Widget::EventHandlerResult::Accepted;
}

}
