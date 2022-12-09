#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Widgets/Checkbox.hpp>
#include <Essa/GUI/Widgets/RadioButton.hpp>
#include <Essa/GUI/Widgets/RadioGroup.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <cstddef>
#include <string>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Checkboxes");

    auto& container1 = host_window.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();

    auto check1 = container1.add_widget<GUI::Checkbox>();
    check1->set_caption("Sample checkbox");
    check1->set_size({ Util::Length::Auto, 15.0_px });

    check1->on_change = [](bool state) {
        if (state)
            std::cout << "CHECKED\n";
        else
            std::cout << "UNCHECKED\n";
    };

    auto check2 = container1.add_widget<GUI::Checkbox>();
    check2->set_caption("Sample checkbox");
    check2->set_size({ Util::Length::Auto, 15.0_px });
    check2->set_style(GUI::Checkbox::Style::MARK);

    check2->on_change = [](bool state) {
        if (state)
            std::cout << "CHECKED\n";
        else
            std::cout << "UNCHECKED\n";
    };

    app.run();
    return 0;
}
