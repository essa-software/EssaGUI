#include "Mouse.hpp"

#include <SDL2/SDL_mouse.h>

namespace llgl
{

Util::Vector2i mouse_position()
{
    int x;
    int y;
    SDL_GetMouseState(&x, &y);
    return { x, y };
}

bool is_mouse_button_pressed(MouseButton button)
{
    int x;
    int y;
    int buttons = SDL_GetMouseState(&x, &y);
    return buttons & SDL_BUTTON(static_cast<int>(button));
}

}
