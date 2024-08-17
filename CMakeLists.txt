cmake_minimum_required(VERSION 3.19)

project(Essa)

execute_process(COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${Essa_SOURCE_DIR}
    OUTPUT_VARIABLE ESSA_GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(GLEW REQUIRED)
find_package(fmt REQUIRED)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# TinyVG
option(ENABLE_TINYVG "Enable TinyVG support" OFF)
if (ENABLE_TINYVG)
    include(cmake/tinyvg.cmake)
    add_compile_definitions(ESSA_ENABLE_TINYVG)
endif()

include(cmake/utils.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(ESSAGUI_EXAMPLES "Compile examples" ON)
option(ESSAGUI_TESTS "Compile tests" ON)
option(ESSAGUI_UTIL_ONLY "Compile only EssaUtil (no GUI or Engine)" OFF)

include_directories(${Essa_SOURCE_DIR})

add_subdirectory(EssaUtil)

if (NOT ESSA_UTIL_ONLY)
    add_subdirectory(EssaTools)
    add_subdirectory(Essa)
else()
    message("Skipping compilation of GUI components")
endif()

if (ESSAGUI_EXAMPLES)
    add_subdirectory(examples)
endif()

enable_testing()

if (ESSAGUI_TESTS)
    add_subdirectory(Tests)
endif()
