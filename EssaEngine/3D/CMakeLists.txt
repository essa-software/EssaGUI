set(SOURCES
    Cube.cpp
    ObjLoader.cpp
    Object3D.cpp
    Shapes.cpp
    Sphere.cpp
)
add_essaengine_library(3D 3d)
target_link_libraries(EssaEngine-3D LLGL-Renderer)
