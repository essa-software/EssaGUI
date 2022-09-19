set(SOURCES
    Cube.cpp
    Model.cpp
    ObjLoader.cpp
    Shapes.cpp
    Sphere.cpp

    Shaders/Basic.cpp
    Shaders/Lighting.cpp
)
add_essaengine_library(3D 3d)
target_link_libraries(EssaEngine-3D EssaGUI)
