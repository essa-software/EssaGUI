#pragma once

#include "TextAlign.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

class Textfield : public Widget {
    sf::String m_content;
    unsigned m_font_size = 15;
    Align m_alignment = Align::CenterLeft;

public:
    explicit Textfield(Container& c)
        : Widget(c) { }

    virtual void draw(GUI::SFMLWindow& window) const override;

    sf::String get_content() const { return m_content; }
    Textfield& set_content(sf::String content) {
        m_content = std::move(content);
        return *this;
    }

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    sf::Vector2f calculate_text_size() const;
};

}
