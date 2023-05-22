#pragma once

#include "Painter.hpp"
#include <Essa/GUI/TextAlign.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>
#include <EssaUtil/UString.hpp>
#include <cstdint>

namespace Gfx {

class Text {
public:
    Text(Util::UString const& string, llgl::TTFFont const& font)
        : m_string(string)
        , m_font(font) { }

    void set_font_size(uint32_t);
    void set_string(Util::UString);
    void set_fill_color(Util::Color);
    void set_position(Util::Point2f);
    void align(GUI::Align, Util::Rectf rect);

    void draw(Gfx::Painter&) const;
    Util::Size2u calculate_text_size() const;
    float find_character_position(size_t index) const;

private:
    void generate_vertices() const;
    Util::Size2u calculate_text_size(Util::UString const&) const;

    Util::UString m_string;
    llgl::TTFFont const& m_font;
    Util::Color m_fill_color = Util::Colors::White;
    Util::Point2f m_position;
    uint32_t m_font_size = 30;
    mutable std::optional<std::vector<Gfx::Vertex>> m_vertices;
};

}
