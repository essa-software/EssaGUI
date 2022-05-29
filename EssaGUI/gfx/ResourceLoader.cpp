#include "ResourceLoader.hpp"

namespace Gfx {

sf::Texture require_texture(std::string const& path) {
    sf::Texture tex;
    if (!tex.loadFromFile(path))
        exit(1);
    return tex;
}

}
