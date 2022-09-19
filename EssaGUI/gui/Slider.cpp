#include "Slider.hpp"

#include "NotifyUser.hpp"
#include <EssaGUI/eml/Loader.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <cmath>
#include <iostream>

namespace GUI {

double Slider::value() const {
    switch (m_mode) {
    case Mode::Linear:
        return m_val;
    case Mode::Exponential:
        return std::pow(m_exponent, m_val);
    }
    return 0;
}

void Slider::set_value(double val, NotifyUser notify_user) {
    m_val = val;

    if (on_change && notify_user == NotifyUser::Yes)
        on_change(value());
}

void Slider::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::MouseButtonPress) {
        if (is_mouse_over(event.event().mouse_button.position)) {
            m_dragging = true;
            event.set_handled();
        }
    }
    else if (event.type() == llgl::Event::Type::MouseButtonRelease) {
        m_dragging = false;
    }
    else if (event.type() == llgl::Event::Type::MouseMove) {
        if (m_dragging) {
            // TODO: Support converting UninitializedVector to any Vector directly
            auto mouse_pos_relative_to_slider = Util::Vector2f { Util::Vector2i { event.event().mouse_move.position } } - raw_position();
            m_val = (mouse_pos_relative_to_slider.x() / raw_size().x()) * (m_max - m_min) + m_min;

            if (m_wraparound) {
                auto middle = (m_min + m_max) / 2;
                m_val = std::remainder(m_val - m_min - middle, (m_max - m_min)) + m_min + middle;
            }
            else
                m_val = std::min(std::max(m_min, m_val), m_max);

            // round to step
            m_val /= m_step;
            m_val = std::round(m_val);
            m_val *= m_step;

            if (on_change)
                on_change(value());
        }
    }
}

float Slider::calculate_knob_size() const {
    return std::max(4.0, raw_size().x() / (m_max - m_min) * m_step);
}

void Slider::draw(GUI::Window& window) const {
    RectangleDrawOptions slider;
    slider.fill_color = are_all_parents_enabled() ? theme().slider.background : theme().slider.background - Util::Color { 60, 60, 60, 0 };
    window.draw_rectangle({ { 0, raw_size().y() / 2 - 2.5f }, { raw_size().x(), 5.f } }, slider);

    RectangleDrawOptions bound;
    bound.fill_color = are_all_parents_enabled() ? theme().slider.background : theme().slider.background - Util::Color { 60, 60, 60, 0 };
    window.draw_rectangle({ { 0, raw_size().y() / 2 - 5 }, Util::Vector2f(2, 10) }, bound);
    window.draw_rectangle({ { raw_size().x() - 2, raw_size().y() / 2 - 5 }, Util::Vector2f(2, 10) }, bound);

    RectangleDrawOptions slider_value;
    auto knob_size_x = calculate_knob_size();
    slider_value.fill_color = are_all_parents_enabled() ? theme().slider.foreground : theme().slider.foreground - Util::Color { 70, 70, 70, 0 };
    window.draw_rectangle(
        { { static_cast<float>((value_clamped_to_min_max() - m_min) / (m_max - m_min) * raw_size().x() - knob_size_x / 2),
              raw_size().y() / 2 - 10.f },
            Util::Vector2f(knob_size_x, 20.f) },
        slider_value);
}

double Slider::value_clamped_to_min_max() const {
    return std::min(std::max(value(), m_min), m_max);
}

EML::EMLErrorOr<void> Slider::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_min = TRY(object.get_property("min", m_min).to_double());
    m_max = TRY(object.get_property("max", m_max).to_double());
    m_step = TRY(object.get_property("step", m_step).to_double());
    m_val = TRY(object.get_property("value", m_val).to_double());

    return {};
}

EML_REGISTER_CLASS(Slider);

}
