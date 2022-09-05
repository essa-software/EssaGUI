#pragma once

#include "Button.hpp"
#include "EssaGUI/gui/Widget.hpp"
#include <EssaUtil/UString.hpp>

namespace GUI {

class RadioButton : public Button {
public:
    virtual void on_init() override { set_toggleable(true); }

    virtual void draw(GUI::Window& window) const override;

    Util::UString get_caption() const { return m_caption; }
    void set_caption(Util::UString const& str) { m_caption = str; }

    virtual Theme::ButtonColors default_button_colors() const override;

private:
    Util::UString m_caption = "";
};

}
