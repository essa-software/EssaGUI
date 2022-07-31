#include "TTFFont.hpp"

#include "ImageLoader.hpp"

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>

namespace llgl {

TTFFont::~TTFFont()
{
    for (auto const& font : m_cached_fonts)
        TTF_CloseFont(font.second);
}

TTFFont TTFFont::open_from_file(std::string const& path)
{
    return TTFFont { path };
}

void ensure_sdl_ttf_initialized()
{
    static bool s_initialized = false;
    if (!s_initialized) {
        if (TTF_Init() < 0) {
            std::cerr << "TTFFont: Failed TTF_Init: " << TTF_GetError() << std::endl;
        }
        s_initialized = true;
    }
}

int TTFFont::ascent(int font_size) const
{
    auto sdl_font = load_font_if_needed(font_size);
    if (!sdl_font) {
        // FIXME: IDK what to do here
        return 0;
    }
    return TTF_FontAscent(sdl_font);
}

int TTFFont::descent(int font_size) const
{
    auto sdl_font = load_font_if_needed(font_size);
    if (!sdl_font) {
        // FIXME: IDK what to do here
        return 0;
    }
    return TTF_FontDescent(sdl_font);
}

int TTFFont::line_height(int font_size) const
{
    auto sdl_font = load_font_if_needed(font_size);
    if (!sdl_font) {
        // FIXME: IDK what to do here
        return 0;
    }
    return TTF_FontLineSkip(sdl_font);
}

std::optional<llgl::Image> TTFFont::render_text(Util::UString const& text, int font_size) const
{
    if (text.is_empty())
        return {};

    auto sdl_font = load_font_if_needed(font_size);
    if (!sdl_font)
        return {};

    auto surface = TTF_RenderUTF8_Blended(sdl_font, text.encode().c_str(), { 255, 255, 255, 255 });
    if (!surface) {
        std::cerr << "TTFFont: Failed TTF_RenderUTF8_Blended: " << TTF_GetError() << std::endl;
        return {};
    }

    auto image = ImageLoader::load_from_sdl_surface(surface);
    SDL_FreeSurface(surface);
    return image;
}

Util::Vector2u TTFFont::calculate_text_size(Util::UString const& text, int font_size) const
{
    if (text.is_empty())
        return {};

    auto sdl_font = load_font_if_needed(font_size);
    if (!sdl_font)
        return {};

    int w;
    int h;
    TTF_SizeUTF8(sdl_font, text.encode().c_str(), &w, &h);
    return { w, h };
}

TTF_Font* TTFFont::load_font_if_needed(int font_size) const
{
    ensure_sdl_ttf_initialized();

    auto it = m_cached_fonts.find(font_size);
    if (it != m_cached_fonts.end())
        return it->second;

    auto sdl_font = TTF_OpenFont(m_path.c_str(), font_size);
    if (!sdl_font) {
        std::cerr << "TTFFont: Failed TTF_OpenFont: " << TTF_GetError() << std::endl;
        return {};
    }
    m_cached_fonts.insert({ font_size, sdl_font });
    return sdl_font;
}

}
