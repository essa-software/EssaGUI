cmake_minimum_required(VERSION 3.19)
project(Essa)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if(ESSAGUI_USE_LOCAL_ESSAUTIL)
    find_package(essautil)
else()
    include(FetchContent)
    FetchContent_Declare(EssaUtil
        GIT_REPOSITORY https://github.com/essa-software/util.git
        GIT_TAG aa9179b3a1a1cda9ba9473a845ba7a57edd13b2f
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(EssaUtil)
endif()

option(ESSAGUI_EXAMPLES "Compile examples" ON)

add_subdirectory(Essa)

install(FILES cmake/EssaConfig.cmake DESTINATION lib/cmake/Essa)
install(DIRECTORY Essa/ DESTINATION include/Essa FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY assets DESTINATION share/Essa)
install(EXPORT Essa DESTINATION lib/cmake/Essa NAMESPACE Essa::)

if(ESSAGUI_EXAMPLES)
    add_subdirectory(examples)
endif()

include(CPack)
