#pragma once

#include <Essa/LLGL/Core/Image.hpp>
#include <optional>
#include <string_view>

class SDL_Surface;

namespace llgl {

class ImageSaver {
public:
    static Util::ErrorOr<void, std::string> save_to_file(Image const&, std::string const& path);
};

}
