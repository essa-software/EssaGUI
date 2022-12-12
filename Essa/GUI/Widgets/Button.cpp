#include "Button.hpp"

#include <iostream>

namespace GUI {

Widget::EventHandlerResult Button::on_mouse_button_press(Event::MouseButtonPress const&) {
    m_pressed_on_button = true;
    return Widget::EventHandlerResult::Accepted;
}

Widget::EventHandlerResult Button::on_mouse_button_release(Event::MouseButtonRelease const&) {
    if (is_hover() && m_pressed_on_button) {
        click();
        m_pressed_on_button = false;
    }
    return Widget::EventHandlerResult::Accepted;
}

Theme::BgFgTextColors Button::colors_for_state() const {
    auto colors = m_button_colors_override.value_or(default_button_colors()).value(*this);
    return colors;
}

void Button::click() {
    if (m_toggleable) {
        m_active = !m_active;
        if (on_change)
            on_change(m_active);
    }
    if (on_click)
        on_click();
}

EML::EMLErrorOr<void> Button::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));
    m_toggleable = TRY(object.get_property("toggleable", m_toggleable).to_bool());
    m_active = TRY(object.get_property("active", m_active).to_bool());
    return {};
}

}
