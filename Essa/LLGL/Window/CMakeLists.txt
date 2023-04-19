set(SOURCES
    Clipboard.cpp
    Impls/SDLHelpers.cpp
    Impls/SDLWindow.cpp
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)
add_llgl_library(Window window)
target_link_libraries(LLGL-Window Util SDL2 Xrender)
