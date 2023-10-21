#include "DefaultResources.hpp"

#include <Essa/LLGL/Resources/ImageLoader.hpp>

namespace Gfx {

std::optional<Texture> ResourceTraits<Texture>::load_from_file(std::string const& path) {
    auto image = llgl::ImageLoader::load_from_file(path);
    if (!image)
        return {};
    return llgl::Texture::create_from_image(*image);
}

std::optional<Font> ResourceTraits<Font>::load_from_file(std::string const& path) { return llgl::TTFFont::open_from_file(path); }

}
