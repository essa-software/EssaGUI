function(add_llgl_test TARGET_NAME)
    add_executable(llgl-test${TARGET_NAME} ${TARGET_NAME}.cpp)
    target_include_directories(llgl-test${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
endfunction()

add_subdirectory(3D)
add_subdirectory(Renderer)
add_subdirectory(Window)
