#include "TTFFont.hpp"

#include "ImageLoader.hpp"

#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <utility>

namespace llgl {

TTFFont::~TTFFont() {
    for (auto const& font : m_cached_fonts)
        TTF_CloseFont(font.second.sdl_font);
}

TTFFont TTFFont::open_from_file(std::string const& path) { return TTFFont { path }; }

void ensure_sdl_ttf_initialized() {
    static bool s_initialized = false;
    if (!s_initialized) {
        if (TTF_Init() < 0) {
            std::cerr << "TTFFont: Failed TTF_Init: " << TTF_GetError() << std::endl;
        }
        s_initialized = true;
    }
}

int TTFFont::ascent(uint32_t font_size) const {
    auto* font_face = load_font_if_needed(font_size);
    if (!font_face)
        return {};
    return TTF_FontAscent(font_face->sdl_font);
}

int TTFFont::descent(uint32_t font_size) const {
    auto* font_face = load_font_if_needed(font_size);
    if (!font_face)
        return {};
    return TTF_FontDescent(font_face->sdl_font);
}

int TTFFont::line_height(uint32_t font_size) const {
    FontFace* font_face = load_font_if_needed(font_size);
    if (!font_face)
        return {};
    return TTF_FontLineSkip(font_face->sdl_font);
}

int TTFFont::kerning(uint32_t font_size, uint32_t previous, uint32_t current) const {
    auto* font_face = load_font_if_needed(font_size);
    if (!font_face)
        return {};
    return TTF_GetFontKerningSizeGlyphs32(font_face->sdl_font, previous, current);
}

std::optional<llgl::Image> TTFFont::render_text(Util::UString const& text, uint32_t font_size) const {
    if (text.is_empty())
        return {};

    auto* font_face = load_font_if_needed(font_size);
    if (!font_face)
        return {};

    auto* surface = TTF_RenderUTF8_Blended(font_face->sdl_font, text.encode().c_str(), { 255, 255, 255, 255 });
    if (!surface) {
        fmt::print("TTFFont: TTF_RenderUTF8_Blended('{}') failed: {}\n", text.encode(), TTF_GetError());
        return {};
    }

    auto image = ImageLoader::load_from_sdl_surface(surface);
    SDL_FreeSurface(surface);
    return image;
}

Util::Size2u TTFFont::calculate_text_size(Util::UString const& text, uint32_t font_size) const {
    if (text.is_empty())
        return {};

    auto* font_face = load_font_if_needed(font_size);
    if (!font_face)
        return {};

    int w;
    int h;
    TTF_SizeUTF8(font_face->sdl_font, text.encode().c_str(), &w, &h);
    return { w, h };
}

GlyphCache* TTFFont::cache(uint32_t font_size) const {
    auto* font_face = load_font_if_needed(font_size);
    return font_face ? &font_face->cache : nullptr;
}

TTFFont::FontFace* TTFFont::load_font_if_needed(uint32_t font_size) const {
    ensure_sdl_ttf_initialized();

    auto it = m_cached_fonts.find(font_size);
    if (it != m_cached_fonts.end())
        return &it->second;

    auto* sdl_font = TTF_OpenFont(m_path.c_str(), static_cast<int>(font_size));
    if (!sdl_font) {
        std::cerr << "TTFFont: Failed TTF_OpenFont from " << m_path << ": " << TTF_GetError() << std::endl;
        return nullptr;
    }
    return &m_cached_fonts.emplace(std::piecewise_construct, std::make_tuple(font_size), std::make_tuple(sdl_font, font_size))
                .first->second;
}

}
