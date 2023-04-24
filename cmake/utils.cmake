function(essautil_setup_target targetname)
    target_compile_options(${targetname} PRIVATE
        -fdiagnostics-color=always
        -Wall -Wextra -Werror
        -Wnon-virtual-dtor
        -Wno-error=missing-field-initializers       # FIXME: This is buggy (?) for *DrawOptions
        -Wno-error=format                           # FIXME: We don't use formatting anyway except SimulationClock but this breaks mingw build
        -Wno-error=stringop-overflow                # FIXME: fmt on CI doesn't like it for some reason, find a way to workaround it!
    )

    set_property(TARGET ${targetname} PROPERTY CXX_STANDARD 20)

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options(${targetname} PRIVATE -ggdb3 -fno-omit-frame-pointer)
        if (ESSA_ENABLE_SANITIZERS)
            message("Enabling sanitizers for ${targetname}")
            target_compile_options(${targetname} PUBLIC -fsanitize=undefined,address -fno-sanitize-recover)
            target_link_options(${targetname} PUBLIC -fsanitize=undefined,address -fno-sanitize-recover)
        endif()
    else()
        target_compile_options(${targetname} PUBLIC -O3)
    endif()
endfunction()

function(essautil_setup_packaging)
    install(SCRIPT "${Essa_SOURCE_DIR}/cmake/setup_packaging.cmake")   
endfunction()

function(essautil_add_test targetname)
    cmake_parse_arguments(PARSE_ARGV 1 essautil_add_test "" "" "LIBS")
    add_executable(${targetname} ${targetname}.cpp)
    target_link_libraries(${targetname} ${essautil_add_test_LIBS})
    target_include_directories(${targetname} PRIVATE ${Essa_SOURCE_DIR})
    essautil_setup_target(${targetname})
endfunction()

function(essa_resources targetname dir)
    install(DIRECTORY assets DESTINATION share/${targetname})
    
    set(outfile ${CMAKE_BINARY_DIR}/EssaResources.cpp)
    file(WRITE ${outfile} "extern \"C\" { const char* ESSA_RESOURCE_DIR = \"")
    file(APPEND ${outfile} ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    file(APPEND ${outfile} "\"; }")

    target_sources(${targetname} PRIVATE ${outfile})
endfunction()
