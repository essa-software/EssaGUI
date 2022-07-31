#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/ToolWindow.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Console.hpp>

int main() {
    GUI::Window wnd { { 1000, 1000 }, "Scrollable widget" };

    GUI::Application app(wnd);

    auto& wnd1 = app.open_overlay<GUI::ToolWindow>();
    wnd1.set_size({ 500, 100 });
    wnd1.center_on_screen();
    auto& container1 = wnd1.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::HorizontalBoxLayout>();
    auto console = container1.add_widget<GUI::Console>();
    console->append_line({ .color = Util::Colors::Red, .text = "test1" });
    console->append_line({ .color = Util::Colors::Yellow, .text = "test2" });
    console->append_line({ .color = Util::Colors::Green, .text = "test3" });
    console->append_line({ .color = Util::Colors::Blue, .text = "test4" });
    console->append_line({ .color = Util::Colors::Red, .text = "test5" });
    console->append_line({ .color = Util::Colors::Yellow, .text = "test6" });
    console->append_line({ .color = Util::Colors::Green, .text = "test7" });
    console->append_line({ .color = Util::Colors::Blue, .text = "test8" });

    app.run();
    return 0;
}
