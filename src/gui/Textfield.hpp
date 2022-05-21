#pragma once

#include "TextAlign.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

class Textfield : public Widget {
    sf::String m_content;
    sf::Color m_bg_color = sf::Color(0, 0, 0, 0);
    sf::Color m_fg_color = sf::Color(0, 0, 255);
    sf::Color m_text_color = sf::Color(255, 255, 255);
    unsigned m_font_size = 15;
    Align m_alignment = Align::CenterLeft;

public:
    explicit Textfield(Container& c)
        : Widget(c) { }

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    virtual void draw(sf::RenderWindow& window) const override;

    sf::String get_content() const { return m_content; }
    Textfield& set_content(sf::String content) {
        m_content = std::move(content);
        return *this;
    }

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };
};

}
