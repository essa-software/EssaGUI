#pragma once

#include "Button.hpp"
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Color.hpp>

namespace GUI {

class ColorPicker : public Button {
public:
    virtual void on_init() override;

    CREATE_VALUE(Util::Color, color, Util::Colors::Black)

    std::function<void(Util::Color)> on_change;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }
    virtual void draw(Gfx::Painter& window) const override;
    virtual LengthVector initial_size() const override {
        return { Util::Length::Auto, { static_cast<float>(theme().line_height), Util::Length::Px } };
    }
};

}
