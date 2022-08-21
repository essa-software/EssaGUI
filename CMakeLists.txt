cmake_minimum_required(VERSION 3.19)
project(EssaGUI)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(EssaUtil REQUIRED)

add_subdirectory(LLGL)
add_subdirectory(EssaEngine)
add_subdirectory(EssaGUI)

target_compile_definitions(EssaGUI PUBLIC ESSAUTIL_ENABLE_SFML)

install(FILES cmake/EssaGUIConfig.cmake DESTINATION lib/cmake/EssaGUI)
install(DIRECTORY EssaEngine/ DESTINATION include/EssaEngine FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY EssaGUI/ DESTINATION include/EssaGUI FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY LLGL/ DESTINATION include/LLGL FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY assets DESTINATION share/EssaGUI)
install(EXPORT EssaGUI DESTINATION lib/cmake/EssaGUI NAMESPACE EssaGUI::)

add_subdirectory(examples)
