#pragma once

#include <Essa/LLGL/Window/Keyboard.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>
#include <EssaUtil/Vector.hpp>

namespace llgl {

class Event {
public:
    enum class Type {
        Unknown,
        Resize,
        KeyPress,
        KeyRelease,
        MouseMove,
        MouseButtonPress,
        MouseButtonRelease,
        MouseScroll,
        TextInput
    } type {};

    struct UninitializedVector2 {
        int x;
        int y;
        operator Util::Vector2i() const { return { x, y }; }
    };

    struct ResizeEvent {
        UninitializedVector2 size;
    };
    struct KeyPressEvent {
        KeyCode keycode;
        bool shift;
        bool ctrl;
        bool alt;
        bool meta;
    };
    struct MouseMoveEvent {
        UninitializedVector2 position;
        UninitializedVector2 relative;
    };
    struct MouseButtonEvent {
        UninitializedVector2 position;
        MouseButton button;
    };
    struct MouseScrollEvent {
        UninitializedVector2 position;
        float delta;
    };
    struct TextInputEvent {
        uint32_t codepoint;
    };

    // TODO: Port this thing to something better than union.
    union {
        ResizeEvent resize;
        KeyPressEvent key;
        MouseMoveEvent mouse_move;
        MouseButtonEvent mouse_button;
        MouseScrollEvent mouse_scroll;
        TextInputEvent text_input;
    };
};

}
