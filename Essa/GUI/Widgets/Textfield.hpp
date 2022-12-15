#pragma once

#include <Essa/GUI/TextAlign.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Units.hpp>

namespace GUI {

class Textfield : public Widget {
public:
    Textfield();

    virtual void draw(Gfx::Painter& window) const override;

    // FIXME: EML
    void set_font(llgl::TTFFont const& font) { m_font = &font; }

    CREATE_VALUE(Util::UString, content, "")
    CREATE_VALUE(size_t, font_size, theme().label_font_size)
    CREATE_VALUE(Align, alignment, Align::CenterLeft)
    CREATE_VALUE(float, padding, 5)

    Util::Rectf text_rect() const;

private:
    virtual LengthVector initial_size() const override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

    llgl::TTFFont const* m_font = nullptr;
};

}
