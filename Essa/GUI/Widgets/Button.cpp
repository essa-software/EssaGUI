#include "Button.hpp"

#include <iostream>

namespace GUI {

Button::Button()
    : m_behavior([&](Util::Cs::Point2i point) {
        return local_rect().contains(point);
    }) {
    m_behavior.on_click = [&]() {
        click();
    };
}

Widget::EventHandlerResult Button::on_mouse_move(Event::MouseMove const& event) {
    return m_behavior.on_mouse_move(event);
}

Widget::EventHandlerResult Button::on_mouse_button_press(Event::MouseButtonPress const& event) {
    return m_behavior.on_mouse_button_press(event);
}

Widget::EventHandlerResult Button::on_mouse_button_release(Event::MouseButtonRelease const& event) {
    return m_behavior.on_mouse_button_release(event);
}

Widget::EventHandlerResult Button::on_key_press(Event::KeyPress const& event) {
    if (event.code() == llgl::KeyCode::Enter) {
        click();
        return Widget::EventHandlerResult::Accepted;
    }
    return Widget::EventHandlerResult::NotAccepted;
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
    m_toggleable = TRY(object.get_property("toggleable", EML::Value(m_toggleable)).to_bool());
    m_active = TRY(object.get_property("active", EML::Value(m_active)).to_bool());
    return {};
}

}
