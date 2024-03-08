#pragma once

#include <Essa/GUI/TextAlign.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>

namespace Gfx {

struct RichTextContext {
    llgl::TTFFont const& default_font;
    Util::Color font_color = Util::Colors::White;
    int font_size = 15;
    GUI::Align text_alignment = GUI::Align::TopLeft;
};

}
