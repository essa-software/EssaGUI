essa_executable(GeometryPlayground SOURCES main.cpp LIBS Essa::GUI)
target_compile_options(GeometryPlayground PRIVATE -Werror=address) #f you
essa_resources(GeometryPlayground assets)
