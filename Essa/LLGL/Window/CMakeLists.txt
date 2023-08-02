set(SOURCES
    Clipboard.cpp
    Impls/SDLHelpers.cpp
    Impls/SDLWindow.cpp
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)
add_llgl_library(Window window)
if(NOT EMSCRIPTEN) # FIXME: if(LINUX)
    set(X11_LIBS Xrender)
endif()
target_link_libraries(LLGL-Window Util SDL2 ${X11_LIBS})
if(EMSCRIPTEN)
    target_compile_options(LLGL-Window PUBLIC "SHELL:-s USE_SDL=2")
    target_link_options(LLGL-Window PUBLIC "SHELL:-s MAX_WEBGL_VERSION=2 -s USE_WEBGL2=1 -s MIN_WEBGL_VERSION=2 -s FULL_ES3=1")
endif()
