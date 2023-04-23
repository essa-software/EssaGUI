add_executable(TransformEditor main.cpp)
essa_resources(TransformEditor assets)
target_link_libraries(TransformEditor Essa::GUI Essa::Engine-3D)
