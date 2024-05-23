#pragma once

#include "ResourceManager.hpp"
#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>

namespace Gfx {

using Font = llgl::TTFFont;
using Texture = llgl::Texture;

template<>
struct ResourceTraits<Font> {
    static std::optional<Font> load_from_file(std::string const&);
    static std::string_view base_path() {
        return "fonts";
    }
};

template<>
struct ResourceTraits<Texture> {
    static std::optional<Texture> load_from_file(std::string const&);
    static std::string_view base_path() {
        return "textures";
    }
};

template<>
struct ResourceTraits<Icon> {
    static std::optional<Icon> load_from_file(std::string const&);
    static std::string_view base_path() {
        return "icons";
    }
};

}
