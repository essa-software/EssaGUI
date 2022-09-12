#pragma once

#include "NotifyUser.hpp"
#include "Widget.hpp"

#include <functional>
#include <string>

namespace GUI {

class Slider : public Widget {
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

    CREATE_VALUE(double, min, 0.0)
    CREATE_VALUE(double, max, 100.0)
    CREATE_VALUE(double, step, 1.0)
    CREATE_VALUE(Mode, mode, Mode::Linear)
    CREATE_VALUE(double, exponent, 2.0)
    CREATE_VALUE(bool, wraparound, false)

    void set_range(double min, double max, double step = 1) {
        set_min(min);
        set_max(max);
        set_step(step);
    }

    double value() const;
    double raw_value() const { return m_val; }
    void set_value(double val, NotifyUser = NotifyUser::Yes);

    virtual void handle_event(Event&) override;
    virtual void draw(GUI::Window& window) const override;

    std::function<void(double)> on_change;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;

    double value_clamped_to_min_max() const;
    float calculate_knob_size() const;

    double m_val = 50;
    bool m_dragging = false;
};

}
