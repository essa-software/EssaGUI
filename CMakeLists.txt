cmake_minimum_required(VERSION 3.19)
project(Essa)

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
