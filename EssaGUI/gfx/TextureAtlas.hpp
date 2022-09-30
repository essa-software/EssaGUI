#pragma once

#include "ResourceManager.hpp"
#include <EssaUtil/Rect.hpp>
#include <LLGL/OpenGL/Texture.hpp>

namespace Gfx {

class TextureAtlas {
public:
    struct Icon {
        llgl::Texture const& texture;
        Util::Rectf texture_rect;
    };

    Icon require(std::string const& name);

private:
    llgl::Texture m_atlas;
    Util::Vector2u m_current_position;
    std::map<std::string, Util::Rectf> m_entries;
};

template<>
struct ResourceTraits<TextureAtlas> {
    static std::optional<TextureAtlas> load_from_file(std::string const&);
    static std::string_view base_path() { return "textures"; }
};

}
