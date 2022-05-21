cmake_minimum_required(VERSION 3.19)
project(EssaGUI)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)

add_library(essa-gui
    src/gfx/ClipViewScope.cpp
    src/gfx/RoundedEdgeRectangleShape.cpp

    src/gui/Application.cpp
    src/gui/ArrowButton.cpp
    src/gui/Button.cpp
    src/gui/ColorPicker.cpp
    src/gui/Console.cpp
    src/gui/Container.cpp
    src/gui/Datebox.cpp
    src/gui/EventLoop.cpp
    src/gui/FileExplorer.cpp
    src/gui/FilePrompt.cpp
    src/gui/Frame.cpp
    src/gui/ImageButton.cpp
    src/gui/ListView.cpp
    src/gui/MessageBox.cpp
    src/gui/Prompt.cpp
    src/gui/SettingsMenu.cpp
    src/gui/SelectWidget.cpp
    src/gui/Slider.cpp
    src/gui/TabWidget.cpp
    src/gui/Textbox.cpp
    src/gui/TextButton.cpp
    src/gui/Textfield.cpp
    src/gui/Tooltip.cpp
    src/gui/ToolWindow.cpp
    src/gui/ValueSlider.cpp
    src/gui/Widget.cpp
    src/gui/WidgetTreeRoot.cpp

    # FIXME: There are many things in util that are used in Essa
    #        but not in GUI. Get rid of them here.
    src/util/SimulationClock.cpp
    src/util/UnitDisplay.cpp
)

target_link_libraries(
    essa-gui
    PUBLIC
    sfml-graphics
)

function(setup_target targetname)
    install(TARGETS ${targetname} EXPORT EssaGUI DESTINATION lib)

    target_compile_options(${targetname} PRIVATE -Wall -Wextra -Werror)
    target_compile_options(${targetname} PRIVATE
        -Wnon-virtual-dtor
        -Wno-potentially-evaluated-expression # this annoys in Container.cpp, enable it when it causes bugs
    )

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options(${targetname} PUBLIC -fsanitize=undefined,address)
        target_link_options(${targetname} PUBLIC -fsanitize=undefined,address)
    else()
        target_compile_options(${targetname} PUBLIC -O3)
    endif()

    target_include_directories(${targetname} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
endfunction()

setup_target(essa-gui)

install(FILES cmake/EssaGUIConfig.cmake DESTINATION lib/cmake/EssaGUI)
install(DIRECTORY src/ DESTINATION include/EssaGUI FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(EXPORT EssaGUI DESTINATION lib/cmake/EssaGUI)
