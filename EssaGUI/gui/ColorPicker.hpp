#pragma once

#include "Button.hpp"

namespace GUI {

class ColorPicker : public Button {
public:
    explicit ColorPicker();

    Util::Color color() const;
    void set_color(Util::Color color);

    std::function<void(Util::Color)> on_change;

private:
    virtual Theme::ButtonColors default_button_colors() const override {
        return theme().text_button;
    }
    virtual void draw(GUI::Window& window) const override;
    virtual LengthVector initial_size() const override { return { Length::Auto, 30.0_px }; }

    Util::Color m_color;
};

}
