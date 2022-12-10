add_library(GUI SHARED
    Application.cpp
    EventLoop.cpp
    HostWindow.cpp
    Model.cpp
    Theme.cpp
    WidgetTreeRoot.cpp

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
    Graphics/RichText/Fragments.cpp
    Graphics/Text.cpp
    Graphics/Window.cpp
    
    Overlays/ContextMenu.cpp
    Overlays/FileExplorer.cpp
    Overlays/FilePrompt.cpp
    Overlays/MessageBox.cpp
    Overlays/Prompt.cpp
    Overlays/Tooltip.cpp
    Overlays/ToolWindow.cpp

    Sprites/RectangleShape.cpp
    Sprites/Sprite.cpp

    Util/ConfigFile.cpp
    
    Widgets/AbstractListView.cpp
    Widgets/ArrowButton.cpp
    Widgets/Border.cpp
    Widgets/Button.cpp
    Widgets/Canvas.cpp
    Widgets/Checkbox.cpp
    Widgets/ColorPicker.cpp
    Widgets/Console.cpp
    Widgets/Container.cpp
    Widgets/Datebox.cpp
    Widgets/DraggableView2D.cpp
    Widgets/Frame.cpp
    Widgets/Grid.cpp
    Widgets/ImageButton.cpp
    Widgets/ImageWidget.cpp
    Widgets/Listbox.cpp
    Widgets/ListView.cpp
    Widgets/Progressbar.cpp
    Widgets/RadioButton.cpp
    Widgets/RadioGroup.cpp
    Widgets/ScrollableWidget.cpp
    Widgets/SelectWidget.cpp
    Widgets/SettingsMenu.cpp
    Widgets/Slider.cpp
    Widgets/TabWidget.cpp
    Widgets/Textbox.cpp
    Widgets/TextButton.cpp
    Widgets/TextEditor.cpp
    Widgets/Textfield.cpp
    Widgets/TreeView.cpp
    Widgets/ValueSlider.cpp
    Widgets/Widget.cpp
    Widgets/WorldView.cpp
)
add_library(Essa::GUI ALIAS GUI)

set_target_properties(GUI PROPERTIES OUTPUT_NAME "essa-gui")
essautil_setup_target(GUI)
install(TARGETS GUI EXPORT Essa DESTINATION lib)
target_link_libraries(GUI PUBLIC essautil LLGL-OpenGL LLGL-Window LLGL-Resources)

# FIXME: This breaks build of eml/AST.cpp somewhere in Value::string()
target_compile_options(GUI PRIVATE -Wno-restrict)