#pragma once

#include <EssaUtil/CoordinateSystem.hpp>
#include <EssaUtil/Enum.hpp>

#include <fmt/core.h>
#include <string_view>

// --- Taken from <SDL2/SDL_mouse.h> ---
#define SDL_BUTTON(X) (1 << ((X)-1))
#define SDL_BUTTON_LEFT 1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT 3
#define SDL_BUTTON_X1 4
#define SDL_BUTTON_X2 5
#define SDL_BUTTON_LMASK SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK SDL_BUTTON(SDL_BUTTON_RIGHT)
#define SDL_BUTTON_X1MASK SDL_BUTTON(SDL_BUTTON_X1)
#define SDL_BUTTON_X2MASK SDL_BUTTON(SDL_BUTTON_X2)
// ---

namespace llgl {

// clang-format off
#define ENUMERATE_MOUSE_BUTTONS(Ex)   \
    Ex(Left, SDL_BUTTON_LEFT)         \
    Ex(Middle, SDL_BUTTON_MIDDLE)     \
    Ex(Right, SDL_BUTTON_RIGHT)
// clang-format on

ESSA_ENUM_WITH_VALUES(MouseButton, ENUMERATE_MOUSE_BUTTONS)
ESSA_ENUM_TO_STRING(MouseButton, ENUMERATE_MOUSE_BUTTONS)

Util::Point2i mouse_position();
bool is_mouse_button_pressed(MouseButton);

}

ESSA_ENUM_FMT_FORMATTER(llgl, MouseButton, ENUMERATE_MOUSE_BUTTONS)
