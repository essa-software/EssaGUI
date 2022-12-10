set(SOURCES
    Cube.cpp
    Model.cpp
    ObjLoader.cpp
    ShaderResource.cpp
    Shapes.cpp
    Sphere.cpp

    Shaders/Basic.cpp
    Shaders/Lighting.cpp
)
add_essaengine_library(3D 3d)
target_link_libraries(Engine-3D GUI)
