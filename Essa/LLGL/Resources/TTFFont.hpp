#pragma once

#include <Essa/LLGL/Core/Image.hpp>
#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <Essa/LLGL/Resources/GlyphCache.hpp>
#include <EssaUtil/NonCopyable.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <map>

using TTF_Font = struct _TTF_Font;

namespace llgl {

class TTFFont : public Util::NonCopyable {
public:
    ~TTFFont();

    TTFFont(TTFFont&& other) = default;
    TTFFont& operator=(TTFFont&& other) = default;

    static TTFFont open_from_file(std::string const& path);

    int ascent(uint32_t font_size) const;
    int descent(uint32_t font_size) const;
    int line_height(uint32_t font_size) const;
    int kerning(uint32_t font_size, uint32_t previous, uint32_t current) const;

    std::optional<llgl::Image> render_text(Util::UString const& text, uint32_t font_size) const;
    Util::Vector2u calculate_text_size(Util::UString const& text, uint32_t font_size) const;
    GlyphCache* cache(uint32_t font_size) const;

private:
    struct FontFace {
        TTF_Font* sdl_font;
        GlyphCache cache;

        FontFace(TTF_Font* font, uint32_t font_size)
            : sdl_font(font)
            , cache(font_size) { }
    };

    FontFace* load_font_if_needed(uint32_t font_size) const;

    explicit TTFFont(std::string const& path)
        : m_path(path) {
    }

    mutable std::map<uint32_t, FontFace> m_cached_fonts;
    std::string m_path;
};

}
