#pragma once

#include "TextAlign.hpp"
#include "Widget.hpp"
#include <EssaUtil/Units.hpp>

namespace GUI {

class Textfield : public Widget {
public:
    virtual void draw(GUI::Window& window) const override;

    CREATE_VALUE(Util::UString, content, "")
    CREATE_VALUE(size_t, font_size, 20)
    CREATE_VALUE(Align, alignment, Align::CenterLeft)
    CREATE_VALUE(float, padding, 5)

    Util::Rectf text_rect() const;

private:
    virtual LengthVector initial_size() const override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;
};

}
