#pragma once

#include "NotifyUser.hpp"
#include "Widget.hpp"

#include <functional>
#include <string>

namespace GUI {

class Slider : public Widget {
    double m_min_val = 0, m_max_val = 100;
    double m_step = 1;
    double m_val = 50;

    bool m_dragging = false;
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

    double min() const { return m_min_val; }
    void set_min(double min) { m_min_val = min; }

    double max() const { return m_max_val; }
    void set_max(double max) { m_max_val = max; }

    double step() const { return m_step; }
    void set_step(double step) { m_step = step; }

    double get_value() const;
    double get_raw_value() const { return m_val; }
    void set_value(double val, NotifyUser = NotifyUser::Yes);

    // NOTE: Mode affects only returned value and display, e.g min/max values
    //       are exponents.
    void set_mode(Mode mode) { m_mode = mode; }
    void set_exponent(double exp) { m_exponent = exp; }
    void set_wraparound(bool wp) { m_wraparound = wp; }

    virtual void handle_event(Event&) override;
    virtual void draw(GUI::Window& window) const override;

    void set_text_attributes(unsigned text_size, std::string string, TextPos text_pos = TextPos::RIGHT);
    void set_range(double min, double max, double step);

    std::function<void(double)> on_change;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;

    double value_clamped_to_min_max() const;
    float calculate_knob_size() const;

    TextPos m_text_pos;
    Mode m_mode = Mode::Linear;
    double m_exponent = 2;
    bool m_wraparound = false;
};

}
