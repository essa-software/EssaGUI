#pragma once
#include "Button.hpp"

namespace GUI {

class Checkbox : public Button {
public:
    explicit Checkbox() { set_toggleable(true); }

    virtual void draw(GUI::Window& window) const override;

    Util::UString get_caption() const { return m_caption; }
    void set_caption(Util::UString const& str) { m_caption = str; }

    virtual Theme::ButtonColors default_button_colors() const override;

    enum class Style {
        CROSS,
        MARK
    };

    Style get_style() const { return m_style; }
    void set_style(const Style& style) { m_style = style; }

private:
    Util::UString m_caption = "";

    Style m_style = Style::CROSS;
};

}
