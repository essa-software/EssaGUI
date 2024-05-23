option(ESSA_ZIG_PATH "Path to the zig executable" "")

# check if zig version is exactly 0.11.0
execute_process(COMMAND "${ESSA_ZIG_PATH}" version
    OUTPUT_VARIABLE ZIG_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT ZIG_VERSION)
    message(FATAL_ERROR "Zig not found, please set ESSA_ZIG_PATH CMake variable to the zig executable")
endif()

if(NOT ZIG_VERSION STREQUAL "0.11.0")
    message(FATAL_ERROR "Zig version =0.11.0 is required, found ${ZIG_VERSION}")
endif()

include(ExternalProject)

# TODO: Support other platforms
if(NOT UNIX)
    message(FATAL_ERROR "TinyVG is only supported on Unix-like systems")
endif()

ExternalProject_Add(tinyvg_build
    GIT_REPOSITORY    git@github.com:TinyVG/sdk.git
    GIT_TAG           24abddf4628b65f2d9d72522562659fe6639d609
    GIT_SHALLOW       TRUE
    GIT_PROGRESS      TRUE
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND "${ESSA_ZIG_PATH}" build
    BUILD_IN_SOURCE   TRUE
    BUILD_BYPRODUCTS  zig-out/lib/libtinyvg.so
)

ExternalProject_Get_Property(tinyvg_build SOURCE_DIR)

set(INCLUDE_DIR ${SOURCE_DIR}/zig-out/include)
# Hack to make it work, otherwise CMake will complain about missing
# include directory.
# (See https://stackoverflow.com/a/47358004)
file(MAKE_DIRECTORY ${INCLUDE_DIR})

add_library(tinyvg INTERFACE)
target_include_directories(tinyvg INTERFACE ${INCLUDE_DIR})
target_link_libraries(tinyvg INTERFACE ${SOURCE_DIR}/zig-out/lib/libtinyvg.so)
add_dependencies(tinyvg tinyvg_build)

install(FILES ${SOURCE_DIR}/zig-out/lib/libtinyvg.so DESTINATION lib)
