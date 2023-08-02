set(SOURCES
    GlyphCache.cpp
    ImageLoader.cpp
    TTFFont.cpp
)
add_llgl_library(Resources resources)
target_link_libraries(LLGL-Resources LLGL-Core LLGL-OpenGL)
if (EMSCRIPTEN)
    target_link_options(LLGL-Resources PUBLIC "SHELL:-s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s SDL2_IMAGE_FORMATS=\'[\"png\",\"jpg\"]\'")
else()
    target_link_libraries(LLGL-Resources SDL2_image SDL2_ttf)
endif()
