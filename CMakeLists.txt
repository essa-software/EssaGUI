cmake_minimum_required(VERSION 3.19)

project(Essa)

execute_process(COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${Essa_SOURCE_DIR}
    OUTPUT_VARIABLE ESSA_GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(GLEW REQUIRED)
find_package(fmt REQUIRED)

include(cmake/utils.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(ESSAGUI_EXAMPLES "Compile examples" ON)

include_directories(${Essa_SOURCE_DIR})

add_subdirectory(EssaTools)
add_subdirectory(EssaUtil)
add_subdirectory(Essa)

if(ESSAGUI_EXAMPLES)
    add_subdirectory(examples)
endif()
