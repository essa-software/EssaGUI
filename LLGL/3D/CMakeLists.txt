set(SOURCES
    Cube.cpp
    ObjLoader.cpp
    Object3D.cpp
    Shapes.cpp
    Sphere.cpp
)
add_llgl_library(3D 3d)
target_link_libraries(3D Renderer)
