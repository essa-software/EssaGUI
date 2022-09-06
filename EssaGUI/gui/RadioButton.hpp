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
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual LengthVector initial_size() const override { return { Length::Auto, { static_cast<float>(theme().line_height), Length::Unit::Px } }; }

    Util::UString m_caption = "";
};

}
