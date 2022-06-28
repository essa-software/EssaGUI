#include "EssaGUI/gui/MessageBox.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/TextEditor.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(200, 200), "TextEditor", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };

    GUI::Application app(wnd);

    auto& container1 = app.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();
    auto text_editor = container1.add_widget<GUI::TextEditor>();
    text_editor->set_placeholder("Test placeholder");

    auto singleline_text_editor = container1.add_widget<GUI::TextEditor>();
    singleline_text_editor->set_multiline(false);
    singleline_text_editor->set_placeholder("Single line test");
    singleline_text_editor->on_enter = [](sf::String const& content) {
        GUI::message_box("You entered: " + content, "Message", GUI::MessageBox::Buttons::Ok);
    };

    app.run();
    return 0;
}
