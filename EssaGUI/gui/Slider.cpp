#include "Slider.hpp"

#include "EssaGUI/gfx/SFMLWindow.hpp"
#include "NotifyUser.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

namespace GUI {

Slider::Slider(Container& c, double min_val, double max_val, double step)
    : Widget(c)
    , m_min_val(min_val)
    , m_max_val(max_val)
    , m_step(step)
    , m_val((min_val + max_val) / 2) {
}

double Slider::get_value() const {
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
        on_change(get_value());
}

void Slider::set_text_attributes(unsigned text_size, std::string string, TextPos text_pos) {
    m_text_size = text_size;
    m_string = string;
    m_text_pos = text_pos;
}

void Slider::handle_event(Event& event) {
    if (event.type() == sf::Event::MouseButtonPressed) {
        if (is_mouse_over({ event.event().mouseButton.x, event.event().mouseButton.y })) {
            m_dragging = true;
            event.set_handled();
        }
    }
    else if (event.type() == sf::Event::MouseButtonReleased) {
        m_dragging = false;
    }
    else if (event.type() == sf::Event::MouseMoved) {
        if (m_dragging) {
            auto mouse_pos_relative_to_slider = sf::Vector2f({ static_cast<float>(event.event().mouseMove.x), static_cast<float>(event.event().mouseMove.y) }) - position();
            m_val = (mouse_pos_relative_to_slider.x / size().x) * (m_max_val - m_min_val) + m_min_val;

            if (m_wraparound) {
                auto middle = (m_min_val + m_max_val) / 2;
                m_val = std::remainder(m_val - m_min_val - middle, (m_max_val - m_min_val)) + m_min_val + middle;
            }
            else
                m_val = std::min(std::max(m_min_val, m_val), m_max_val);

            // round to step
            m_val /= m_step;
            m_val = std::round(m_val);
            m_val *= m_step;

            if (on_change)
                on_change(get_value());
        }
    }
}

float Slider::calculate_knob_size() const {
    return std::max(4.0, size().x / (m_max_val - m_min_val) * m_step);
}

void Slider::draw(GUI::SFMLWindow& window) const {
    RectangleDrawOptions slider;
    slider.fill_color = are_all_parents_enabled() ? get_background_color() : get_background_color() - sf::Color(60, 60, 60, 0);
    window.draw_rectangle({ { 0, size().y / 2 - 2.5f }, { size().x, 5.f } }, slider);

    RectangleDrawOptions bound;
    bound.fill_color = are_all_parents_enabled() ? get_background_color() : get_background_color() - sf::Color(60, 60, 60, 0);
    window.draw_rectangle({ { 0, size().y / 2 - 5 }, sf::Vector2f(2, 10) }, bound);
    window.draw_rectangle({ { size().x - 2, size().y / 2 - 5 }, sf::Vector2f(2, 10) }, bound);

    RectangleDrawOptions slider_value;
    auto knob_size_x = calculate_knob_size();
    slider_value.fill_color = are_all_parents_enabled() ? get_foreground_color() : get_foreground_color() - sf::Color(70, 70, 70, 0);
    window.draw_rectangle(
        { { static_cast<float>((value_clamped_to_min_max() - m_min_val) / (m_max_val - m_min_val) * size().x - knob_size_x / 2),
              size().y / 2 - 10.f },
            sf::Vector2f(knob_size_x, 20.f) },
        slider_value);
}

double Slider::value_clamped_to_min_max() const {
    return std::min(std::max(get_value(), m_min_val), m_max_val);
}

void Slider::set_range(double min, double max, double step) {
    m_min_val = min;
    m_max_val = max;
    m_step = step;
}

}
