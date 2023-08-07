
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>

int main() {
    GUI::Application app;
    GUI::message_box(
        "Test\nmultiline\nAND VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY LONG LINE\nmessage", "test",
        {
            .buttons = GUI::MessageBox::Buttons::Ok,
            .icon = GUI::MessageBox::Icon::Warning,
        }
    );
    return 0;
}
