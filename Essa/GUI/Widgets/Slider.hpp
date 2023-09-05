#pragma once

#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <functional>
#include <string>

namespace GUI {

class Slider : public Widget {
public:
    enum class Mode { Linear, Exponential };

    enum class TextPos { TOP, RIGHT, BOTTOM, LEFT };

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

    virtual void draw(Gfx::Painter& window) const override;

    std::function<void(double)> on_change;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;

    virtual Widget::EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const& event) override;
    virtual Widget::EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const& event) override;
    virtual Widget::EventHandlerResult on_mouse_move(Event::MouseMove const& event) override;

    double value_clamped_to_min_max() const;
    float calculate_knob_size() const;
    void round_to_step();
    virtual bool accepts_focus() const override { return true; }
    virtual LengthVector initial_size() const override {
        return { Util::Length::Auto, { static_cast<float>(theme().line_height), Util::Length::Px } };
    }

    double m_val = 50;
    bool m_dragging = false;
};

}
