set(SOURCES
    BatchRenderer.cpp
    CoreRenderer.cpp
    Renderer.cpp
    RenderToTexture.cpp
    StateScope.cpp
    Transform.cpp
)
add_llgl_library(Renderer renderer)
target_link_libraries(Renderer Window OpenGL)
