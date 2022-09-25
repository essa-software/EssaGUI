#pragma once

#include "Painter.hpp"
#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaUtil/UString.hpp>
#include <LLGL/Resources/TTFFont.hpp>
#include <cstdint>

namespace Gfx {

class Text {
public:
    Text(Util::UString const& string, llgl::TTFFont const& font)
        : m_string(string)
        , m_font(font) { }

    void set_font_size(uint32_t f) { m_font_size = f; }
    void set_string(Util::UString s) { m_string = std::move(s); }
    void set_fill_color(Util::Color c) { m_fill_color = c; }
    void set_position(Util::Vector2f position) { m_position = position; }
    void align(GUI::Align align, Util::Rectf rect);

    void draw(Gfx::Painter&) const;
    Util::Vector2u calculate_text_size() const;
    float find_character_position(size_t index) const;

private:
    Util::Vector2u calculate_text_size(Util::UString const&) const;

    Util::UString m_string;
    llgl::TTFFont const& m_font;
    Util::Color m_fill_color = Util::Colors::White;
    Util::Vector2f m_position;
    uint32_t m_font_size = 30;
};

}
