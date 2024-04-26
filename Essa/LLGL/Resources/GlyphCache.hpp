#pragma once

#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <EssaUtil/Rect.hpp>
#include <map>

namespace llgl {

class TTFFont;

class GlyphCache {
public:
    explicit GlyphCache(uint32_t font_size);

    struct Glyph {
        Util::Rectu texture_rect;
    };

    Glyph ensure_glyph(TTFFont const& font, uint32_t codepoint);
    Texture const& atlas() const {
        return m_atlas;
    }

private:
    uint32_t m_font_size {};
    Texture m_atlas;
    std::map<uint32_t, Glyph> m_glyphs;
    Util::Point2u m_current_atlas_position;
    int m_max_row_height = 0;
};

}
