#pragma once

#include "Button.hpp"
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/UString.hpp>

namespace GUI {

class RadioButton : public Button {
public:
    virtual void on_init() override { set_toggleable(true); }

    virtual void draw(Gfx::Painter& window) const override;

    CREATE_VALUE(Util::UString, caption, "")

    virtual Theme::ButtonColors default_button_colors() const override;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual LengthVector initial_size() const override { return { Util::Length::Auto, { static_cast<int>(theme().line_height), Util::Length::Px } }; }
};

}
