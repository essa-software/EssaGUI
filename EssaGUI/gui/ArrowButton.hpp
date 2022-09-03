#pragma once

#include "Button.hpp"

namespace GUI {

class ArrowButton : public Button {
public:
    enum class ArrowType {
        LEFTARROW,
        BOTTOMARROW,
        RIGHTARROW,
        TOPARROW
    };

    void set_arrow_type(ArrowType type) { m_arrow_type = type; }
    void set_arrow_size(double size) { m_arrow_size = size; }

    ArrowType arrow_type() const { return m_arrow_type; }
    double arrow_size() const { return m_arrow_size; }

private:
    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }

    ArrowType m_arrow_type = ArrowType::TOPARROW;
    double m_arrow_size = 8;
    virtual void draw(GUI::Window&) const override;
};

}
