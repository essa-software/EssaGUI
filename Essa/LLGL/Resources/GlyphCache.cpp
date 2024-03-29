#include "GlyphCache.hpp"

#include "TTFFont.hpp"

namespace llgl {

GlyphCache::GlyphCache(uint32_t font_size)
    : m_font_size(font_size)
    , m_atlas(Texture::create_empty({ 1024, 1024 })) {
    m_atlas.set_filtering(Texture::Filtering::Linear);
}

GlyphCache::Glyph GlyphCache::ensure_glyph(TTFFont const& font, uint32_t codepoint) {
    auto it = m_glyphs.find(codepoint);
    if (it != m_glyphs.end())
        return it->second;

    auto rendered_glyph = font.render_text(Util::UString { codepoint }, m_font_size);
    if (!rendered_glyph)
        return {};

    if (rendered_glyph->size().x() > m_atlas.size().x() || rendered_glyph->size().y() > m_atlas.size().y()) {
        std::cout << "GlyphCache: Atlas is too small to fit glyph cp=" << codepoint << " size=" << m_font_size << std::endl;
        return {};
    }

    if (m_current_atlas_position.x() + rendered_glyph->size().x() > m_atlas.size().x()) {
        m_current_atlas_position.set_x(0);
        m_current_atlas_position.set_y(m_current_atlas_position.y() + m_max_row_height);
        m_max_row_height = 0;

        if (m_current_atlas_position.y() > m_atlas.size().y()) {
            std::cout << "GlyphCache: Not enough space in atlas for glyph cp=" << codepoint << " size=" << m_font_size << std::endl;
            return {};
        }
    }

    Util::Rectu texture_rect {
        m_current_atlas_position,
        rendered_glyph->size(),
    };
    m_atlas.update(
        texture_rect.position(), texture_rect.size(), std::span<Util::Color const> { rendered_glyph->pixels() }, Texture::Format::RGBA
    );

    m_current_atlas_position.set_x(m_current_atlas_position.x() + rendered_glyph->size().x());
    m_max_row_height = std::max<int>(rendered_glyph->size().y(), m_max_row_height);

    Glyph glyph { .texture_rect = texture_rect };
    m_glyphs.insert({ codepoint, glyph });
    return glyph;
}

}
