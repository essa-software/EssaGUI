set(SOURCES
    GlyphCache.cpp
    ImageLoader.cpp
    ImageSaver.cpp
    TTFFont.cpp
)
add_llgl_library(Resources resources)
target_link_libraries(LLGL-Resources LLGL-Core LLGL-OpenGL SDL2_image SDL2_ttf)
