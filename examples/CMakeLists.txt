function(add_example targetname)
    add_executable(${targetname} ${targetname}.cpp)
    target_link_libraries(${targetname} essa-gui)
endfunction()

add_example("file-explorer")
add_example("gfx-test")
add_example("message-box-multiline")
