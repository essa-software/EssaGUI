set(SOURCES
    Error.cpp
    Extensions.cpp
    FBO.cpp
    Projection.cpp
    Renderer.cpp
    Shader.cpp
    Texture.cpp
    Transform.cpp
)
add_llgl_library(OpenGL opengl)
target_link_libraries(LLGL-OpenGL LLGL-Core GL GLEW)
