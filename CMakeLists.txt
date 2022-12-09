cmake_minimum_required(VERSION 3.19)
project(Essa)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(EssaUtil REQUIRED)

option(ESSAGUI_EXAMPLES "Compile examples" ON)

add_subdirectory(Essa)

install(FILES cmake/EssaConfig.cmake DESTINATION lib/cmake/Essa)
install(DIRECTORY Essa/ DESTINATION include/Essa FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY assets DESTINATION share/Essa)
install(EXPORT Essa DESTINATION lib/cmake/Essa NAMESPACE Essa::)

if(ESSAGUI_EXAMPLES)
    add_subdirectory(examples)
endif()
