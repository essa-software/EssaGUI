# TODO: Implement portable installs

option(ESSA_FORCE_DEVEL "Force building in Development mode even on Release" OFF)

unset(ESSA_IS_PRODUCTION CACHE)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND NOT ESSA_FORCE_DEVEL)
    set(ESSA_IS_PRODUCTION 1 CACHE INTERNAL "Is production")
    message("Building in production mode")
else()
    set(ESSA_IS_PRODUCTION 0 CACHE INTERNAL "Is production")
    message("Building in development mode")
endif()

unset(ESSA_IS_SUBPROJECT CACHE)
if("${Essa_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    set(ESSA_IS_SUBPROJECT 0 CACHE INTERNAL "Is built as subproject")
else()
    set(ESSA_IS_SUBPROJECT 1 CACHE INTERNAL "Is built as subproject")
endif()

set(BUILTIN_DIR_INSTALLED 0 CACHE INTERNAL "BUILTIN_DIR_INSTALLED")

# Setup target with Essa-compatible defaults.
function(essautil_setup_target targetname)
    if(NOT BUILTIN_DIR_INSTALLED)
        install(DIRECTORY ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../assets/ DESTINATION share/Essa/builtin)
        set(BUILTIN_DIR_INSTALLED 1 CACHE INTERNAL "BUILTIN_DIR_INSTALLED")
    endif()

    set(ESSA_INSTALL_ASSET_ROOT ${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}/${targetname})
    if (ESSA_IS_PRODUCTION)
        set(ESSA_BUILTIN_ASSET_ROOT {})
    else()
        set(ESSA_BUILTIN_ASSET_ROOT "\"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../assets\"")
    endif()
    set(ESSA_TARGET_NAME ${targetname})

    get_target_property(target_type ${targetname} TYPE)
    if (target_type MATCHES ".*_LIBRARY")
        # FIXME: What to do with MODULE_LIBRARY ????
        install(TARGETS ${targetname} DESTINATION lib)
    else()
        install(TARGETS ${targetname} DESTINATION bin)
        configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/BuildConfig.cpp BuildConfig.cpp)
        target_sources(${targetname} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.cpp)
        if (ESSA_IS_PRODUCTION)
            set_target_properties(${targetname} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
        endif()
    endif()

    target_include_directories(${targetname} PUBLIC ${PROJECT_SOURCE_DIR})

    target_compile_options(${targetname} PRIVATE
        -fdiagnostics-color=always
        -Wall -Wextra -Werror
        -Wnon-virtual-dtor
        -Wno-error=missing-field-initializers       # FIXME: This is buggy (?) for *DrawOptions
        -Wno-error=format                           # FIXME: We don't use formatting anyway except SimulationClock but this breaks mingw build
        -Wno-error=stringop-overflow                # FIXME: fmt on CI doesn't like it for some reason, find a way to workaround it!
        -Wno-error=deprecated-declarations
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

# Add an Essa-compatible executable.
#
# Options:
# - DEV_ONLY        Don't build & install in production mode. May be used for examples, tests, ...
function(essa_executable targetname)
    cmake_parse_arguments(PARSE_ARGV 1 EE "DEV_ONLY" "" "SOURCES;LIBS")
    if (EE_DEV_ONLY AND (ESSA_IS_PRODUCTION OR ESSA_IS_SUBPROJECT))
        # message("Skipping DEV_ONLY target ${targetname}")
        return()
    endif()
    add_executable(${targetname} ${EE_SOURCES})
    # FIXME: Link essa here?
    essautil_setup_target(${targetname})
    target_link_libraries(${targetname} ${EE_LIBS})
endfunction()

function(essautil_setup_packaging)
    install(SCRIPT "${Essa_SOURCE_DIR}/cmake/setup_packaging.cmake")   
endfunction()

function(essautil_add_test targetname)
    cmake_parse_arguments(PARSE_ARGV 1 EAT "NO_CTEST" "" "LIBS")
    essa_executable(Test${targetname} SOURCES ${targetname}.cpp LIBS ${EAT_LIBS} DEV_ONLY)
    if (NOT EAT_NO_CTEST)
        add_test(NAME Test${targetname} COMMAND Test${targetname})
    endif()
    if (NOT ESSA_IS_PRODUCTION AND NOT ESSA_IS_SUBPROJECT)
        target_include_directories(Test${targetname} PRIVATE ${Essa_SOURCE_DIR})
    endif()
endfunction()

function(essa_resources targetname dir)
    set(DEST_PATH share/${CMAKE_PROJECT_NAME}/${targetname})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/ DESTINATION ${DEST_PATH})
    
    set(outfile ${CMAKE_CURRENT_BINARY_DIR}/${targetname}-EssaResources.cpp)
    file(WRITE ${outfile} "extern \"C\" { const char* ESSA_RESOURCE_DIR = \"")
    if (ESSA_IS_PRODUCTION)
        file(APPEND ${outfile} ${CMAKE_INSTALL_PREFIX}/${DEST_PATH})
    else()
        file(APPEND ${outfile} ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    endif()
    file(APPEND ${outfile} "\"; }")

    target_sources(${targetname} PRIVATE ${outfile})
endfunction()
