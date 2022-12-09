#pragma once

#include <Essa/LLGL/Core/Image.hpp>
#include <optional>
#include <string_view>

class SDL_Surface;

namespace llgl {

class ImageLoader {
public:
    static std::optional<Image> load_from_file(std::string const& path);
    static Image load_from_sdl_surface(SDL_Surface*);
};

}
