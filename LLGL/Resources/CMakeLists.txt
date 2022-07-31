set(SOURCES
    ImageLoader.cpp
    TTFFont.cpp
)
add_llgl_library(Resources resources)
target_link_libraries(LLGL-Resources LLGL-Core SDL2_image SDL2_ttf)
