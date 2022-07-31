#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/TextEditor.hpp>
#include <EssaGUI/gui/Textbox.hpp>

int main() {
    GUI::Window wnd { { 500, 500 }, "TextEditor" };

    GUI::Application app(wnd);

    auto& container1 = app.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();
    auto text_editor = container1.add_widget<GUI::TextEditor>();
    text_editor->set_placeholder("Test placeholder");

    auto number_text_box = container1.add_widget<GUI::Textbox>();
    number_text_box->set_placeholder("Enter number");
    number_text_box->on_enter = [](Util::UString const& content) {
        GUI::message_box("You enteRed number: " + content, "Message", GUI::MessageBox::Buttons::Ok);
    };

    auto text_box = container1.add_widget<GUI::Textbox>();
    text_box->set_placeholder("Enter text");
    text_box->set_data_type(GUI::Textbox::Type::TEXT);
    text_box->on_enter = [](Util::UString const& content) {
        GUI::message_box("You enteRed text: " + content, "Message", GUI::MessageBox::Buttons::Ok);
    };

    app.run();
    return 0;
}
