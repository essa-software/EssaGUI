#pragma once
#include "Button.hpp"
#include "NotifyUser.hpp"
#include "TextAlign.hpp"
#include "Widget.hpp"

namespace GUI {

class TextButton : public Button {
public:
    void set_content(Util::UString content) { m_content = std::move(content); }
    void set_active_content(Util::UString content) { m_active_content = std::move(content); }

    void set_image(llgl::opengl::Texture* image) { m_image = image; }

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    Util::UString content() const { return m_content; }
    Util::UString active_content() const { return m_content; }

private:
    virtual void draw(GUI::Window& window) const override;

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;
    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }
    virtual LengthVector initial_size() const override { return { Length::Auto, { static_cast<float>(theme().line_height), Length::Unit::Px } }; }

    Util::UString m_content;
    Util::UString m_active_content;
    unsigned m_font_size = 20;
    Align m_alignment = Align::Center;
    llgl::opengl::Texture* m_image = nullptr;
};

}
