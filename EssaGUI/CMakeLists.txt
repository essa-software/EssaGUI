add_library(EssaGUI SHARED
    EML/AST.cpp
    EML/EMLObject.cpp
    EML/EMLResource.cpp
    EML/Lexer.cpp
    EML/Loader.cpp
    EML/Parser.cpp
    EML/SyntaxHighlighter.cpp

    Graphics/ClipViewScope.cpp
    Graphics/DefaultGUIShader.cpp
    Graphics/Painter.cpp
    Graphics/ResourceManager.cpp
    Graphics/RichText.cpp
    Graphics/Text.cpp
    Graphics/Window.cpp
    Graphics/RichText/Fragments.cpp

    GUI/AbstractListView.cpp
    GUI/Application.cpp
    GUI/ArrowButton.cpp
    GUI/Border.cpp
    GUI/Button.cpp
    GUI/Canvas.cpp
    GUI/Checkbox.cpp
    GUI/ColorPicker.cpp
    GUI/Console.cpp
    GUI/Container.cpp
    GUI/ContextMenu.cpp
    GUI/Datebox.cpp
    GUI/DraggableView2D.cpp
    GUI/EventLoop.cpp
    GUI/FileExplorer.cpp
    GUI/FilePrompt.cpp
    GUI/Frame.cpp
    GUI/Grid.cpp
    GUI/HostWindow.cpp
    GUI/ImageButton.cpp
    GUI/ImageWidget.cpp
    GUI/Listbox.cpp
    GUI/ListView.cpp
    GUI/MessageBox.cpp
    GUI/Model.cpp
    GUI/Progressbar.cpp
    GUI/Prompt.cpp
    GUI/RadioButton.cpp
    GUI/RadioGroup.cpp
    GUI/ScrollableWidget.cpp
    GUI/SettingsMenu.cpp
    GUI/SelectWidget.cpp
    GUI/Slider.cpp
    GUI/TabWidget.cpp
    GUI/TextButton.cpp
    GUI/TextEditor.cpp
    GUI/Textbox.cpp
    GUI/Textfield.cpp
    GUI/Theme.cpp
    GUI/ToolWindow.cpp
    GUI/Tooltip.cpp
    GUI/TreeView.cpp
    GUI/ValueSlider.cpp
    GUI/Widget.cpp
    GUI/WidgetTreeRoot.cpp
    GUI/WorldView.cpp

    GUI/Sprites/RectangleShape.cpp
    GUI/Sprites/Sprite.cpp
    
    Util/ConfigFile.cpp
)

set_target_properties(EssaGUI PROPERTIES OUTPUT_NAME "essa-gui")
essautil_setup_target(EssaGUI)
install(TARGETS EssaGUI EXPORT EssaGUI DESTINATION lib)
target_link_libraries(EssaGUI PUBLIC essautil LLGL-OpenGL LLGL-Window LLGL-Resources)

# FIXME: This breaks build of eml/AST.cpp somewhere in Value::string()
target_compile_options(EssaGUI PRIVATE -Wno-restrict)
