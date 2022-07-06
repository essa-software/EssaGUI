#pragma once

#include "Container.hpp"
#include "Slider.hpp"
#include "StateTextButton.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include <EssaUtil/UnitDisplay.hpp>

namespace GUI {

class UnitSlider : public Container {
public:
    UnitSlider(Container& parent, Util::Quantity q, double min, double max, double step = 1);

    void set_name(std::string name) { m_name_textfield->set_content(std::move(name)); }

    double value() const;
    void set_value(double value);

    Slider& slider() { return *m_slider; }

    std::function<void(double)> on_change;

private:
    virtual LengthVector initial_size() const override { return { Length::Auto, 32.0_px }; }

    Textfield* m_name_textfield = nullptr;
    Slider* m_slider = nullptr;
    Textbox* m_textbox = nullptr;
    StateTextButton<unsigned>* m_unit_button = nullptr;

    Util::Quantity m_unit;
};

}
