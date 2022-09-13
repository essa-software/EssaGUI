#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Multiline Message Box");
    GUI::message_box(host_window, "Test\nmultiline\nAND VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY LONG LINE\nmessage", "test", GUI::MessageBox::Buttons::Ok);
    return 0;
}
