#pragma once
#include "Button.hpp"

namespace GUI {

class Checkbox : public Button {
public:
    explicit Checkbox(Container& c)
        : Button(c) { set_toggleable(true); }

    virtual void draw(GUI::Window& window) const override;

    Util::UString get_caption() const { return m_caption; }
    void set_caption(Util::UString const& str) { m_caption = str; }

    virtual Theme::ButtonColors default_button_colors() const override;

private:
    Util::UString m_caption = "";
};

}
