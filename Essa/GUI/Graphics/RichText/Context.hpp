#pragma once

#include <Essa/LLGL/Resources/TTFFont.hpp>

namespace Gfx {

struct RichTextContext {
    llgl::TTFFont const& default_font;
    int font_size = 15;
};

}
