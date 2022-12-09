#pragma once

#include <Essa/LLGL/Core/Image.hpp>
#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <Essa/LLGL/Resources/GlyphCache.hpp>
#include <EssaUtil/NonCopyable.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <SDL2/SDL_ttf.h>
#include <map>

using TTF_Font = struct _TTF_Font;

namespace llgl {

class TTFFont : public Util::NonCopyable {
public:
    ~TTFFont();

    TTFFont(TTFFont&& other) = default;
    TTFFont& operator=(TTFFont&& other) = default;

    static TTFFont open_from_file(std::string const& papth);

    int ascent(int font_size) const;
    int descent(int font_size) const;
    int line_height(int font_size) const;
    int kerning(int font_size, uint32_t previous, uint32_t current) const;

    std::optional<llgl::Image> render_text(Util::UString const& text, int font_size) const;
    Util::Vector2u calculate_text_size(Util::UString const& text, int font_size) const;
    GlyphCache* cache(int font_size) const;

private:
    struct FontFace {
        TTF_Font* sdl_font;
        GlyphCache cache;

        FontFace(TTF_Font* font, int font_size)
            : sdl_font(font)
            , cache(font_size) { }
    };

    FontFace* load_font_if_needed(int font_size) const;

    explicit TTFFont(std::string const& path)
        : m_path(path) {
    }

    mutable std::map<int, FontFace> m_cached_fonts;
    std::string m_path;
};

}
