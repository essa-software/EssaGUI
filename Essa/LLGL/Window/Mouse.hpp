#pragma once

#include <EssaUtil/CoordinateSystem.hpp>
#include <EssaUtil/Enum.hpp>

#include <SDL2/SDL_mouse.h>
#include <fmt/format.h>
#include <string_view>

namespace llgl {

// clang-format off
#define ENUMERATE_MOUSE_BUTTONS(Ex)   \
    Ex(Left, SDL_BUTTON_LEFT)         \
    Ex(Middle, SDL_BUTTON_MIDDLE)     \
    Ex(Right, SDL_BUTTON_RIGHT)
// clang-format on

ESSA_ENUM_WITH_VALUES(MouseButton, ENUMERATE_MOUSE_BUTTONS)
ESSA_ENUM_TO_STRING(MouseButton, ENUMERATE_MOUSE_BUTTONS)

Util::Cs::Point2i mouse_position();
bool is_mouse_button_pressed(MouseButton);

}

ESSA_ENUM_FMT_FORMATTER(llgl, MouseButton, ENUMERATE_MOUSE_BUTTONS)
