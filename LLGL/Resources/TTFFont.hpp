#pragma once

#include <EssaUtil/UString.hpp>
#include <LLGL/Core/Image.hpp>
#include <map>

using TTF_Font = struct _TTF_Font;

namespace llgl {

class TTFFont {
public:
    ~TTFFont();

    static TTFFont open_from_file(std::string const& papth);

    int ascent(int font_size) const;
    int descent(int font_size) const;
    int line_height(int font_size) const;
    std::optional<llgl::Image> render_text(Util::UString const& text, int font_size) const;
    Util::Vector2u calculate_text_size(Util::UString const& text, int font_size) const;

private:
    TTF_Font* load_font_if_needed(int font_size) const;

    explicit TTFFont(std::string const& path)
        : m_path(path)
    {
    }

    mutable std::map<int, TTF_Font*> m_cached_fonts;
    std::string m_path;
};

}
