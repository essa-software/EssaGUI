#include <EssaGUI/Application.hpp>
#include <EssaGUI/Widgets/ColorPicker.hpp>
#include <EssaGUI/Widgets/Container.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Color picker");

    auto& container = host_window.set_main_widget<GUI::Container>();
    container.set_layout<GUI::VerticalBoxLayout>().set_padding(GUI::Boxf::all_equal(10));

    container.add_widget<GUI::ColorPicker>();

    app.run();
    return 0;
}
