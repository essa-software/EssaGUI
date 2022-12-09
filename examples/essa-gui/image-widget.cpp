#include <EssaGUI/Application.hpp>
#include <EssaGUI/Widgets/Console.hpp>
#include <EssaGUI/Widgets/Container.hpp>
#include <EssaGUI/Widgets/ImageWidget.hpp>
#include <EssaGUI/Overlays/ToolWindow.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 1000, 1000 }, "Image Widget");

    auto& container1 = host_window.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::HorizontalBoxLayout>();
    auto image_widget = container1.add_widget<GUI::ImageWidget>();

    image_widget->set_image(&app.resource_manager().require_texture("gui/directory.png"));

    app.run();
    return 0;
}
