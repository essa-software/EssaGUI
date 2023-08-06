cmake_minimum_required(VERSION 3.19)

project(Essa)

execute_process(COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${Essa_SOURCE_DIR}
    OUTPUT_VARIABLE ESSA_GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT EMSCRIPTEN)
    find_package(GLEW REQUIRED)
endif()
find_package(fmt REQUIRED)

include(cmake/utils.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(ESSAGUI_EXAMPLES "Compile examples" ON)
option(ESSAGUI_UTIL_ONLY "Compile only EssaUtil (no GUI or Engine)" OFF)

include_directories(${Essa_SOURCE_DIR})
if (EMSCRIPTEN)
    include_directories(${fmt_DIR}/../../../include)
endif()

add_subdirectory(EssaUtil)

if (NOT ESSA_UTIL_ONLY)
    add_subdirectory(EssaTools)
    add_subdirectory(Essa)
else()
    message("Skipping compilation of GUI components")
endif()

if(ESSAGUI_EXAMPLES)
    add_subdirectory(examples)
endif()
