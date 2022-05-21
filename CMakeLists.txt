cmake_minimum_required(VERSION 3.19)
project(EssaGUI)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)

add_library(essa-gui
    EssaGUI/gfx/ClipViewScope.cpp
    EssaGUI/gfx/RoundedEdgeRectangleShape.cpp

    EssaGUI/gui/Application.cpp
    EssaGUI/gui/ArrowButton.cpp
    EssaGUI/gui/Button.cpp
    EssaGUI/gui/ColorPicker.cpp
    EssaGUI/gui/Console.cpp
    EssaGUI/gui/Container.cpp
    EssaGUI/gui/Datebox.cpp
    EssaGUI/gui/EventLoop.cpp
    EssaGUI/gui/FileExplorer.cpp
    EssaGUI/gui/FilePrompt.cpp
    EssaGUI/gui/Frame.cpp
    EssaGUI/gui/ImageButton.cpp
    EssaGUI/gui/ListView.cpp
    EssaGUI/gui/MessageBox.cpp
    EssaGUI/gui/Prompt.cpp
    EssaGUI/gui/SettingsMenu.cpp
    EssaGUI/gui/SelectWidget.cpp
    EssaGUI/gui/Slider.cpp
    EssaGUI/gui/TabWidget.cpp
    EssaGUI/gui/Textbox.cpp
    EssaGUI/gui/TextButton.cpp
    EssaGUI/gui/Textfield.cpp
    EssaGUI/gui/Tooltip.cpp
    EssaGUI/gui/ToolWindow.cpp
    EssaGUI/gui/ValueSlider.cpp
    EssaGUI/gui/Widget.cpp
    EssaGUI/gui/WidgetTreeRoot.cpp

    # FIXME: There are many things in util that are used in Essa
    #        but not in GUI. Get rid of them here.
    EssaGUI/util/SimulationClock.cpp
    EssaGUI/util/UnitDisplay.cpp
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
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
endfunction()

setup_target(essa-gui)

install(FILES cmake/EssaGUIConfig.cmake DESTINATION lib/cmake/EssaGUI)
install(DIRECTORY EssaGUI/ DESTINATION include/EssaGUI FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(EXPORT EssaGUI DESTINATION lib/cmake/EssaGUI)
