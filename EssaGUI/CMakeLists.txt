add_library(EssaGUI SHARED
    eml/AST.cpp
    eml/EMLObject.cpp
    eml/EMLResource.cpp
    eml/Lexer.cpp
    eml/Loader.cpp
    eml/Parser.cpp

    gfx/ClipViewScope.cpp
    gfx/DefaultGUIShader.cpp
    gfx/Painter.cpp
    gfx/ResourceManager.cpp
    gfx/RichText.cpp
    gfx/Text.cpp
    gfx/Window.cpp
    gfx/RichText/Fragments.cpp

    gui/AbstractListView.cpp
    gui/Application.cpp
    gui/ArrowButton.cpp
    gui/Border.cpp
    gui/Button.cpp
    gui/Canvas.cpp
    gui/Checkbox.cpp
    gui/ColorPicker.cpp
    gui/Console.cpp
    gui/Container.cpp
    gui/ContextMenu.cpp
    gui/Datebox.cpp
    gui/EventLoop.cpp
    gui/FileExplorer.cpp
    gui/FilePrompt.cpp
    gui/Frame.cpp
    gui/Grid.cpp
    gui/HostWindow.cpp
    gui/ImageButton.cpp
    gui/ImageWidget.cpp
    gui/Listbox.cpp
    gui/ListView.cpp
    gui/MessageBox.cpp
    gui/Model.cpp
    gui/Progressbar.cpp
    gui/Prompt.cpp
    gui/RadioButton.cpp
    gui/RadioGroup.cpp
    gui/ScrollableWidget.cpp
    gui/SettingsMenu.cpp
    gui/SelectWidget.cpp
    gui/Slider.cpp
    gui/TabWidget.cpp
    gui/TextButton.cpp
    gui/TextEditor.cpp
    gui/Textbox.cpp
    gui/Textfield.cpp
    gui/Theme.cpp
    gui/ToolWindow.cpp
    gui/Tooltip.cpp
    gui/TreeView.cpp
    gui/ValueSlider.cpp
    gui/Widget.cpp
    gui/WidgetTreeRoot.cpp
    gui/WorldView.cpp

    gui/sprites/RectangleShape.cpp
    gui/sprites/Sprite.cpp
    
    util/ConfigFile.cpp
)

set_target_properties(EssaGUI PROPERTIES OUTPUT_NAME "essa-gui")
essautil_setup_target(EssaGUI)
install(TARGETS EssaGUI EXPORT EssaGUI DESTINATION lib)
target_link_libraries(EssaGUI PUBLIC essautil LLGL-OpenGL LLGL-Window LLGL-Resources)

# FIXME: This breaks build of eml/AST.cpp somewhere in Value::string()
target_compile_options(EssaGUI PRIVATE -Wno-restrict)
