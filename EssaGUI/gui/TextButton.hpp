#pragma once
#include "Button.hpp"
#include "NotifyUser.hpp"
#include "TextAlign.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

class TextButton : public Button {
public:
    explicit TextButton(Container& c);

    void set_content(Util::UString content) { m_content = std::move(content); }
    void set_active_content(Util::UString content) { m_active_content = std::move(content); }

    void set_image(sf::Texture* image) { m_image = image; }

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    Util::UString content() const { return m_content; }
    Util::UString active_content() const { return m_content; }

private:
    virtual void draw(GUI::SFMLWindow& window) const override;

    virtual Theme::ButtonColors default_button_colors() const override { return theme().text_button; }

    Util::UString m_content;
    Util::UString m_active_content;
    unsigned m_font_size = 20;
    Align m_alignment = Align::Center;
    sf::Texture* m_image = nullptr;
};

}
