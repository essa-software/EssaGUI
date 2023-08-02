#pragma once

#include <Essa/GUI/Graphics/RichText.hpp>
#include <Essa/GUI/TextAlign.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Units.hpp>

namespace GUI {

class Textfield : public Widget {
public:
    Textfield();

    virtual void draw(Gfx::Painter&) const override;

    // FIXME: EML
    void set_font(llgl::TTFFont const& font) { m_font = &font; }
    void set_content(Gfx::RichText content) {
        m_content = std::move(content);
    }
    void set_content(Util::UString string) {
        m_content = std::move(string);
    }
    CREATE_VALUE(size_t, font_size, theme().label_font_size)
    CREATE_VALUE(Align, alignment, Align::CenterLeft)
    CREATE_VALUE(int, padding, 5)

    Util::Size2u needed_size_for_text() const;

private:
    Util::Recti text_rect() const;
    virtual LengthVector initial_size() const override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

    llgl::TTFFont const* m_font = nullptr;
    std::variant<Util::UString, Gfx::RichText> m_content;
};

}
