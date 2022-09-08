#include "EssaGUI/gui/Application.hpp"
#include "EssaGUI/gui/Container.hpp"
#include <EssaGUI/gui/ColorPicker.hpp>

int main() {
    GUI::Window window { { 500, 500 }, "Color picker" };

    GUI::Application app { window };

    auto& container = app.set_main_widget<GUI::Container>();
    container.set_layout<GUI::VerticalBoxLayout>().set_padding(GUI::Boxf::all_equal(10));

    container.add_widget<GUI::ColorPicker>();

    app.run();
    return 0;
}
