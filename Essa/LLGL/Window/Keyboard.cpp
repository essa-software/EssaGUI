#include "Keyboard.hpp"

#include <SDL2/SDL_keyboard.h>
#include <cassert>

namespace llgl
{

bool is_key_pressed(KeyCode key)
{
    int numkeys {};
    auto state = SDL_GetKeyboardState(&numkeys);
    auto scancode = SDL_GetScancodeFromKey(static_cast<SDL_Scancode>(key));
    assert(static_cast<int>(scancode) < numkeys);
    return state[scancode];
}

}
