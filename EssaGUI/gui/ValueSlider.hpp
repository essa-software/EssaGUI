#pragma once

#include "Container.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"

namespace GUI {

struct ValueSliderOptions {
    Length name_textfield_size = 100.0_px;
    Length unit_textfield_size = 50.0_px;
};

class ValueSlider : public Container {
public:
    ValueSlider(double min, double max, double step = 1, ValueSliderOptions options = {});

    void set_name(Util::UString name) { m_name_textfield->set_content(std::move(name)); }
    void set_unit(Util::UString unit) { m_unit_textfield->set_content(std::move(unit)); }

    double value() const;
    void set_value(double value, NotifyUser = NotifyUser::Yes);

    Slider& slider() { return *m_slider; }

    std::function<void(double)> on_change;

private:
    virtual LengthVector initial_size() const override { return { Length::Auto, { static_cast<float>(theme().line_height), Length::Unit::Px } }; }

    Textfield* m_name_textfield = nullptr;
    Slider* m_slider = nullptr;
    Textbox* m_textbox = nullptr;
    Textfield* m_unit_textfield = nullptr;
};

}
