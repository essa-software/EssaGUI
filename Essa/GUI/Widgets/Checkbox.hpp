#pragma once

#include "Button.hpp"
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>

namespace GUI {

class Checkbox : public Button {
public:
    virtual void on_init() override { set_toggleable(true); }

    virtual void draw(Gfx::Painter& window) const override;

    CREATE_VALUE(Util::UString, caption, "")

    virtual Theme::ButtonColors default_button_colors() const override;

    enum class Style { CROSS, MARK };

    CREATE_VALUE(Style, style, Style::CROSS)

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual LengthVector initial_size() const override {
        return { Util::Length::Auto, { static_cast<float>(theme().line_height), Util::Length::Px } };
    }
};

}
