#pragma once

#include <EssaUtil/Vector.hpp>
#include <SDL2/SDL_mouse.h>
#include <string_view>

namespace llgl {

#define ENUMERATE_MOUSE_BUTTONS(K) \
    K(Left, SDL_BUTTON_LEFT)       \
    K(Middle, SDL_BUTTON_MIDDLE)   \
    K(Right, SDL_BUTTON_RIGHT)

enum class MouseButton {
#define __ENUMERATE_BUTTON(key, value) key = value,
    ENUMERATE_MOUSE_BUTTONS(__ENUMERATE_BUTTON)
#undef __ENUMERATE_BUTTON
};

constexpr std::string_view to_string(MouseButton key)
{
    switch (key) {
#define __ENUMERATE_BUTTON(key, value) \
    case MouseButton::key:             \
        return #key;
        ENUMERATE_MOUSE_BUTTONS(__ENUMERATE_BUTTON)
#undef __ENUMERATE_BUTTON
    }
    return "Unknown";
}

Util::Vector2i mouse_position();
bool is_mouse_button_pressed(MouseButton);

}
