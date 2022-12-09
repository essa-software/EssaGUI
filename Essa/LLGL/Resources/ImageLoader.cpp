#include "ImageLoader.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <iostream>

namespace llgl {

// https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
static Uint32 get_pixel(SDL_Surface& surface, Util::Vector2u position)
{
    int bpp = surface.format->BytesPerPixel;
    Uint8* p = reinterpret_cast<uint8_t*>(surface.pixels) + position.y() * surface.pitch + position.x() * bpp;

    switch (bpp) {
        case 1:
            return *p;
            break;
        case 2:
            return *reinterpret_cast<uint16_t*>(p);
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;
        case 4:
            return *reinterpret_cast<uint32_t*>(p);
            break;
        default:
            __builtin_unreachable();
    }
}

std::optional<Image> ImageLoader::load_from_file(std::string const& path)
{
    auto surface = IMG_Load(path.c_str());
    if (!surface)
        return {};

    auto image = load_from_sdl_surface(surface);
    SDL_FreeSurface(surface);
    return image;
}

Image ImageLoader::load_from_sdl_surface(SDL_Surface* surface)
{
    Image image = Image::create_uninitialized({ surface->w, surface->h });
    for (size_t x = 0; x < image.size().x(); x++) {
        for (size_t y = 0; y < image.size().y(); y++) {
            Util::Color color;
            SDL_GetRGBA(get_pixel(*surface, { x, y }), surface->format, &color.r, &color.g, &color.b, &color.a);
            image.set_pixel({ x, y }, color);
        }
    }
    return image;
}

}
