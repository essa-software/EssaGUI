#pragma once

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <string>

namespace llgl::Detail {

struct SDLWindowData {
    SDL_Window* window {};
    bool focused = false;
};

}
