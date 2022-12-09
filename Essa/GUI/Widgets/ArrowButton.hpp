#pragma once

#include "Button.hpp"
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Units.hpp>

namespace GUI {

class ArrowButton : public Button {
public:
    enum class ArrowType {
        LEFTARROW,
        BOTTOMARROW,
        RIGHTARROW,
        TOPARROW
    };

    CREATE_VALUE(ArrowType, arrow_type, ArrowType::LEFTARROW)
    CREATE_VALUE(Util::Length, arrow_size, 8.0_px)

private:
    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;
    virtual void draw(Gfx::Painter&) const override;
};

}
