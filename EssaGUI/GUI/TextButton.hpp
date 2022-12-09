#pragma once
#include "Button.hpp"
#include "NotifyUser.hpp"
#include "TextAlign.hpp"
#include "Widget.hpp"
#include <EssaUtil/UString.hpp>

namespace GUI {

class TextButton : public Button {
public:
    CREATE_VALUE(Util::UString, content, "")
    CREATE_VALUE(Util::UString, active_content, "")
    CREATE_VALUE(llgl::Texture const*, image, nullptr)
    CREATE_VALUE(size_t, font_size, 20)
    CREATE_VALUE(Align, alignment, Align::Center)

private:
    virtual void draw(Gfx::Painter& window) const override;

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;
    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }
    virtual LengthVector initial_size() const override { return { Util::Length::Auto, { static_cast<float>(theme().line_height), Util::Length::Px } }; }
};

}
