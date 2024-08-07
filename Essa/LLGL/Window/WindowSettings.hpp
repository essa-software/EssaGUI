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
    Shaped = 0x40,
    Tooltip = 0x80,
    PopupMenu = 0x100,
};

ESSA_ENUM_BITWISE_OPERATORS(WindowFlags)

struct ContextSettings {
    int major_version = 3;
    int minor_version = 2;
    int msaa_samples = 0; // 0 means no MSAA
};

struct WindowSettings {
    WindowFlags flags = WindowFlags::Resizable;
    ContextSettings context_settings = {};
};

}
