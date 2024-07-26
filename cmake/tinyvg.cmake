option(ESSA_ZIG_PATH "Path to the zig executable" "")

execute_process(COMMAND "${ESSA_ZIG_PATH}" version
    OUTPUT_VARIABLE ZIG_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT ZIG_VERSION)
    message(FATAL_ERROR "Zig not found, please set ESSA_ZIG_PATH CMake variable to the zig executable")
endif()

if(NOT ZIG_VERSION STREQUAL "0.13.0")
    message(FATAL_ERROR "Zig version =0.13.0 is required, found ${ZIG_VERSION}")
endif()

include(ExternalProject)

# TODO: Support other platforms
if(NOT UNIX)
    message(FATAL_ERROR "TinyVG is only supported on Unix-like systems")
endif()

ExternalProject_Add(tinyvg_ext
    GIT_REPOSITORY    git@github.com:TinyVG/sdk.git
    GIT_TAG           d103ec3ec461a2f96b8a7e7c1dda8ff54d4c8825
    GIT_SHALLOW       TRUE
    GIT_PROGRESS      TRUE
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND "${ESSA_ZIG_PATH}" build
    BUILD_IN_SOURCE   TRUE
    BUILD_BYPRODUCTS  zig-out/lib/libtinyvg.so
)

# get source dir
ExternalProject_Get_Property(tinyvg_ext SOURCE_DIR)

set(tinyvg_path ${SOURCE_DIR}/zig-out/lib/libtinyvg.so)

add_library(tinyvg SHARED IMPORTED GLOBAL)
add_dependencies(tinyvg tinyvg_ext)
set_target_properties(tinyvg
    PROPERTIES IMPORTED_LOCATION ${tinyvg_path}
)
target_include_directories(tinyvg INTERFACE ${SOURCE_DIR}/src/binding/include)

install(FILES ${tinyvg_path} DESTINATION lib)
