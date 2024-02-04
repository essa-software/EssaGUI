#include "ImageSaver.hpp"

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>

namespace llgl {

Util::ErrorOr<void, std::string> ImageSaver::save_to_file(Image const& image, std::string const& path) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, image.size().x(), image.size().y(), 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        return std::string("Failed to create SDL surface: ") + SDL_GetError();
    }

    memcpy(surface->pixels, image.pixels().data(), image.pixels().size_bytes());
    if (IMG_SavePNG(surface, path.c_str()) == -1) {
        SDL_FreeSurface(surface);
        return std::string("Failed to save image to file: ") + SDL_GetError();
    }
    SDL_FreeSurface(surface);
    return {};
}

}
