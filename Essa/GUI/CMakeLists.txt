add_library(GUI SHARED
    Application.cpp
    EventLoop.cpp
    Debug.cpp
    HostWindow.cpp
    Model.cpp
    Theme.cpp
    ThemeRenderer.cpp
    Timer.cpp
    WidgetTreeRoot.cpp
    WindowRoot.cpp

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
    
    Overlays/ContextMenu.cpp
    Overlays/FileExplorer.cpp
    Overlays/FilePrompt.cpp
    Overlays/MessageBox.cpp
    Overlays/NotificationWindow.cpp
    Overlays/Prompt.cpp
    Overlays/Tooltip.cpp

    Sprites/RectangleShape.cpp
    Sprites/Sprite.cpp

    Util/ConfigFile.cpp
    
    Widgets/AbstractListView.cpp
    Widgets/ArrowButton.cpp
    Widgets/Border.cpp
    Widgets/Button.cpp
    Widgets/ButtonBehavior.cpp
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
    Widgets/ScrollableContainer.cpp
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

    Widgets/MDI/Host.cpp
    Widgets/MDI/Overlay.cpp
    Widgets/MDI/Window.cpp
)
add_library(Essa::GUI ALIAS GUI)

set_target_properties(GUI PROPERTIES OUTPUT_NAME "essa-gui")
set_target_properties(GUI PROPERTIES SOVERSION ${ESSA_GIT_REVISION})
essautil_setup_target(GUI)
target_link_libraries(GUI PUBLIC Util LLGL-OpenGL LLGL-Window LLGL-Resources)

# FIXME: This breaks build of eml/AST.cpp somewhere in Value::string()
target_compile_options(GUI PRIVATE -Wno-restrict)
# FIXME: This breaks build of RadioGroup::add_widget().
target_compile_options(GUI PUBLIC -Wno-address)
