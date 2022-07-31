cmake_minimum_required(VERSION 3.19)
project(EssaGUI)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(GLEW REQUIRED)
find_package(EssaUtil REQUIRED)

add_subdirectory(LLGL)

add_library(EssaGUI
    EssaGUI/gfx/DefaultGUIShader.cpp
    EssaGUI/gfx/ClipViewScope.cpp
    EssaGUI/gfx/ResourceManager.cpp
    EssaGUI/gfx/Window.cpp

    EssaGUI/gui/Application.cpp
    EssaGUI/gui/ArrowButton.cpp
    EssaGUI/gui/Button.cpp
    EssaGUI/gui/Checkbox.cpp
    EssaGUI/gui/ColorPicker.cpp
    EssaGUI/gui/Console.cpp
    EssaGUI/gui/Container.cpp
    EssaGUI/gui/ContextMenu.cpp
    EssaGUI/gui/Datebox.cpp
    EssaGUI/gui/EventLoop.cpp
    EssaGUI/gui/FileExplorer.cpp
    EssaGUI/gui/FilePrompt.cpp
    EssaGUI/gui/Frame.cpp
    EssaGUI/gui/ImageButton.cpp
    EssaGUI/gui/ImageWidget.cpp
    EssaGUI/gui/Listbox.cpp
    EssaGUI/gui/ListView.cpp
    EssaGUI/gui/MessageBox.cpp
    EssaGUI/gui/Progressbar.cpp
    EssaGUI/gui/Prompt.cpp
    EssaGUI/gui/RadioButton.cpp
    EssaGUI/gui/RadioGroup.cpp
    EssaGUI/gui/ScrollableWidget.cpp
    EssaGUI/gui/SettingsMenu.cpp
    EssaGUI/gui/SelectWidget.cpp
    EssaGUI/gui/Slider.cpp
    EssaGUI/gui/TabWidget.cpp
    EssaGUI/gui/TextButton.cpp
    EssaGUI/gui/TextEditor.cpp
    EssaGUI/gui/Textbox.cpp
    EssaGUI/gui/Textfield.cpp
    EssaGUI/gui/Theme.cpp
    EssaGUI/gui/ToolWindow.cpp
    EssaGUI/gui/Tooltip.cpp
    EssaGUI/gui/ValueSlider.cpp
    EssaGUI/gui/Widget.cpp
    EssaGUI/gui/WidgetTreeRoot.cpp
    
    EssaGUI/util/ConfigFile.cpp
)

if(MINGW)
    target_link_libraries(EssaGUI PUBLIC essautil opengl32 GLEW::GLEW)
else()
    target_link_libraries(EssaGUI PUBLIC essautil GL GLEW::GLEW EssaGUI::LLGL-Renderer EssaGUI::LLGL-Resources)
endif()

function(setup_target targetname)
    install(TARGETS ${targetname} EXPORT EssaGUI DESTINATION lib)
    essautil_setup_target(${targetname})
endfunction()

setup_target(EssaGUI)
target_compile_definitions(EssaGUI PUBLIC ESSAUTIL_ENABLE_SFML)

install(FILES cmake/EssaGUIConfig.cmake DESTINATION lib/cmake/EssaGUI)
install(DIRECTORY EssaGUI/ DESTINATION include/EssaGUI FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY LLGL/ DESTINATION include/LLGL FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY assets DESTINATION share/EssaGUI)
install(EXPORT EssaGUI DESTINATION lib/cmake/EssaGUI NAMESPACE EssaGUI::)

add_subdirectory(examples)
