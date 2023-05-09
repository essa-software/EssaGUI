essa_executable(TransformEditor SOURCES main.cpp LIBS Essa::GUI Essa::Engine-3D)
target_compile_options(TransformEditor PRIVATE -Werror=address) #f you
essa_resources(TransformEditor assets)
