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

    void set_content(sf::String content) { m_content = std::move(content); }
    void set_active_content(sf::String content) { m_active_content = std::move(content); }

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    sf::String content() const { return m_content; }
    sf::String active_content() const { return m_content; }

private:
    virtual void draw(sf::RenderWindow& window) const override;

    sf::String m_content;
    sf::String m_active_content;
    unsigned m_font_size = 20;
    Align m_alignment = Align::Center;
};

}
