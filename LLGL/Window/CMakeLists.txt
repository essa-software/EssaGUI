set(SOURCES
    Impls/SDLWindow.cpp
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)
add_llgl_library(Window window)
target_link_libraries(LLGL-Window essautil SDL2 LLGL-Renderer)
