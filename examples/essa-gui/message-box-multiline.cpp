#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>

int main() {
    GUI::Window wnd { { 1000, 1000 }, "Multiline msgbox" };
    GUI::Application app(wnd);
    GUI::message_box(app.host_window(), "Test\nmultiline\nAND VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY LONG LINE\nmessage", "test", GUI::MessageBox::Buttons::Ok);
    return 0;
}
