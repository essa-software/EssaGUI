#pragma once

#include "Container.hpp"
#include "Slider.hpp"

namespace GUI {

class ColorPickerDisplay : public Widget {
public:
    explicit ColorPickerDisplay(Container& c)
        : Widget(c) { }

    void set_color(Util::Color color) { m_color = color; }

private:
    Util::Color m_color;

    virtual void draw(GUI::Window& window) const override;
};

class ColorPicker : public Container {
public:
    explicit ColorPicker(Container& c);

    Util::Color value() const;
    void set_value(Util::Color color);

    std::function<void(Util::Color)> on_change;

private:
    Slider* m_r_slider = nullptr;
    Slider* m_g_slider = nullptr;
    Slider* m_b_slider = nullptr;
    ColorPickerDisplay* m_color_picker_display = nullptr;
};

}
