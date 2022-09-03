#include "EssaGUI/gui/MessageBox.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/TextButton.hpp>
#include <EssaUtil/Error.hpp>
#include <EssaUtil/GenericParser.hpp>

int main() {
    GUI::Window window { { 500, 500 }, "EML test" };
    GUI::Application app { window };

    if (!app.load_from_eml_file("../examples/essa-gui/eml-test.eml"))
        return 1;

    static_cast<GUI::Container*>(app.main_widget())->find_widget_of_type_by_id<GUI::TextButton>("test")->on_click = []() {
        GUI::message_box("eml works yay", "test", GUI::MessageBox::Buttons::Ok);
    };
    app.run();
    return 0;
}
