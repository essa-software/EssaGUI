#pragma once

#include "NotifyUser.hpp"
#include "Widget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <string>

namespace GUI {

class Slider : public Widget {
    double m_min_val, m_max_val;
    double m_step;
    double m_val;


    bool m_dragging = false;

    sf::Color m_bg_color = sf::Color(200, 200, 200);
    sf::Color m_fg_color = sf::Color::White;
    sf::Color m_text_color = sf::Color::White;
    unsigned m_text_size;
    std::string m_string;

public:
    enum class Mode {
        Linear,
        Exponential
    };

    enum class TextPos {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT
    };

    Slider(Container&, double min_val, double max_val, double step = 1);
    double get_value() const;
    double get_raw_value() const { return m_val; }
    void set_value(double val, NotifyUser = NotifyUser::Yes);

    // NOTE: Mode affects only returned value and display, e.g min/max values
    //       are exponents.
    void set_mode(Mode mode) { m_mode = mode; }
    void set_exponent(double exp) { m_exponent = exp; }
    void set_wraparound(bool wp) { m_wraparound = wp; }

    virtual void handle_event(Event&) override;
    virtual void draw(GUI::SFMLWindow& window) const override;

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color);
    void set_text_attributes(unsigned text_size, std::string string, TextPos text_pos = TextPos::RIGHT);
    void set_range(double min, double max, double step);

    double step() const { return m_step; }

    std::function<void(double)> on_change;

private:
    double value_clamped_to_min_max() const;

    float calculate_knob_size() const;

    TextPos m_text_pos;
    Mode m_mode = Mode::Linear;
    double m_exponent = 2;
    bool m_wraparound = false;
};

}
