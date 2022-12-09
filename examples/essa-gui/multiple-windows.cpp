#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>

int main() {
    GUI::Application app;

    auto& wnd1 = app.create_host_window({ 250, 250 }, "Window 1");
    auto& btn1 = wnd1.set_main_widget<GUI::TextButton>();
    btn1.set_content("Siema");
    auto& wnd2 = app.create_host_window({ 250, 250 }, "Window 2");
    auto& btn2 = wnd2.set_main_widget<GUI::TextButton>();
    btn2.set_content("tej");

    app.run();
    return 0;
}
