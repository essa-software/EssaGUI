#include "ResourceLoader.hpp"

#include <LLGL/Resources/ImageLoader.hpp>
#include <iostream>

namespace Gfx {

llgl::opengl::Texture require_texture(std::string const& path) {
    auto image = llgl::ImageLoader::load_from_file(path);
    if (!image) {
        std::cout << "ResourceLoader: Failed to load required texture from " << path << ". Aborting" << std::endl;
        exit(1);
    }
    return llgl::opengl::Texture::create_from_image(*image);
}

}
