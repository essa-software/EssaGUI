#pragma once

#include "Button.hpp"

namespace GUI {

class ArrowButton : public Button {
public:
    explicit ArrowButton(Container& c)
        : Button(c) { }

    enum class ArrowType {
        LEFTARROW,
        BOTTOMARROW,
        RIGHTARROW,
        TOPARROW
    };

    void set_arrow_color(Util::Color color) { m_arrow_color = color; }
    void set_arrow_type(ArrowType type) { m_arrow_type = type; }
    void set_arrow_size(double size) { m_arrow_size = size; }

    Util::Color arrow_color() const { return m_arrow_color; }
    ArrowType arrow_type() const { return m_arrow_type; }
    double arrow_size() const { return m_arrow_size; }

private:
    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }

    Util::Color m_arrow_color = Util::Color { 200, 200, 200 };
    ArrowType m_arrow_type = ArrowType::TOPARROW;
    double m_arrow_size = 8;
    virtual void draw(GUI::Window&) const override;
};

}
