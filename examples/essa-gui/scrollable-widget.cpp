#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Console.hpp>
#include <Essa/GUI/Widgets/Container.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 1000, 1000 }, "Scrollable Widget");

    auto& console = host_window.set_root_widget<GUI::Console>();
    console.append_line({ .color = Util::Colors::Red, .text = "test1" });
    console.append_line({ .color = Util::Colors::Yellow, .text = "test2" });
    console.append_line({ .color = Util::Colors::Green, .text = "test3" });
    console.append_line({ .color = Util::Colors::Blue, .text = "test4" });
    console.append_line({ .color = Util::Colors::Red, .text = "test5" });
    console.append_line({ .color = Util::Colors::Yellow, .text = "test6" });
    console.append_line({ .color = Util::Colors::Green, .text = "test7" });
    console.append_line({ .color = Util::Colors::Blue, .text = "test8" });

    app.run();
    return 0;
}
