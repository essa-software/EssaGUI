#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Console.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/ImageWidget.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 1000, 1000 }, "Image Widget");

    auto& container1 = host_window.set_root_widget<GUI::Container>();
    container1.set_layout<GUI::HorizontalBoxLayout>();
    auto image_widget = container1.add_widget<GUI::ImageWidget>();

    image_widget->set_image(&app.resource_manager().require_texture("gui/directory.png"));

    app.run();
    return 0;
}
