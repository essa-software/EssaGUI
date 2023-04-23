#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/ColorPicker.hpp>
#include <Essa/GUI/Widgets/Container.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Color picker");

    auto& container = host_window.set_main_widget<GUI::Container>();
    container.set_layout<GUI::VerticalBoxLayout>().set_padding(GUI::Boxi::all_equal(10));

    container.add_widget<GUI::ColorPicker>();

    app.run();
    return 0;
}
