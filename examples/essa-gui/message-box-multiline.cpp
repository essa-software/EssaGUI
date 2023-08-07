
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>

int main() {
    GUI::Application app;
    GUI::message_box(
        "Test\nmultiline\nAND VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY LONG LINE\nmessage", "test",
        GUI::MessageBox::Buttons::Ok
    );
    return 0;
}
