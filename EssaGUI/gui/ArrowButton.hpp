#pragma once

#include "Button.hpp"
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

    void set_arrow_type(ArrowType type) { m_arrow_type = type; }
    void set_arrow_size(Length size) { m_arrow_size = size; }

    ArrowType arrow_type() const { return m_arrow_type; }
    Length arrow_size() const { return m_arrow_size; }

private:
    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;

    ArrowType m_arrow_type = ArrowType::TOPARROW;
    Length m_arrow_size = 8.0_px;
    virtual void draw(GUI::Window&) const override;
};

}
