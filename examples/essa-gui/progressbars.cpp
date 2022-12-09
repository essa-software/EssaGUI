#include <EssaGUI/GUI/Application.hpp>
#include <EssaGUI/GUI/Checkbox.hpp>
#include <EssaGUI/GUI/MessageBox.hpp>
#include <EssaGUI/GUI/Progressbar.hpp>
#include <EssaGUI/GUI/RadioButton.hpp>
#include <EssaGUI/GUI/RadioGroup.hpp>
#include <EssaGUI/GUI/TextEditor.hpp>
#include <EssaGUI/GUI/Textbox.hpp>
#include <EssaGUI/GUI/Widget.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <cstddef>
#include <ratio>
#include <string>
#include <thread>
#include <vector>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Progress bars");

    auto& container1 = host_window.set_main_widget<GUI::Container>();
    container1.set_background_color(Util::Colors::White);
    container1.set_layout<GUI::VerticalBoxLayout>();

    std::vector<std::thread> thread_vec;

    auto create_progressbar = [&](Util::Color color, size_t timestep, GUI::Container& con, Util::UString const& content, GUI::Progressbar::Labelling label) {
        auto prog1 = con.add_widget<GUI::Progressbar>();
        prog1->set_max(100);
        prog1->set_content(content);
        prog1->set_size({ Util::Length::Auto, 30.0_px });
        prog1->set_progressbar_color(color);
        prog1->set_labelling(label);
        // prog1->set_step(1);

        prog1->on_finish = [content]() {
            std::cout << "Finished " + content << "\n";
        };

        auto busy_progressbar = [](GUI::Progressbar* prog, size_t time) {
            while (!prog->finished()) {
                std::this_thread::sleep_for(std::chrono::duration<size_t, std::milli>(time));
                prog->step_by();
            }
        };

        thread_vec.push_back(std::thread(busy_progressbar, prog1, timestep));
    };

    create_progressbar(Util::Colors::Green, 100, container1, "Processbar1", GUI::Progressbar::Labelling::PERCENTAGE);
    create_progressbar(Util::Colors::Blue, 200, container1, "Processbar2", GUI::Progressbar::Labelling::VALUE);
    create_progressbar(Util::Colors::Red, 50, container1, "Processbar3", GUI::Progressbar::Labelling::NONE);
    create_progressbar(Util::Colors::Yellow, 150, container1, "Processbar4", GUI::Progressbar::Labelling::PERCENTAGE);
    create_progressbar(Util::Colors::Cyan, 300, container1, "Processbar5", GUI::Progressbar::Labelling::NONE);

    app.run();

    return 0;
}
