#pragma once

#include <EssaUtil/Enum.hpp>

namespace llgl {

enum class WindowFlags {
    Fullscreen = 0x01,
    Borderless = 0x02,
    Resizable = 0x04,
    Minimized = 0x08,
    Maximized = 0x10,
    TransparentBackground = 0x20,
};

ESSA_ENUM_BITWISE_OPERATORS(WindowFlags)

struct ContextSettings {
    int major_version = 3;
    int minor_version = 2;
};

struct WindowSettings {
    WindowFlags flags = WindowFlags::Resizable;
    ContextSettings context_settings = {};
};

}
