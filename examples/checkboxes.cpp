#include "EssaGUI/gui/Checkbox.hpp"
#include "EssaGUI/gui/RadioButton.hpp"
#include "EssaGUI/gui/RadioGroup.hpp"
#include "EssaGUI/gui/Widget.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/TextEditor.hpp>
#include <EssaGUI/gui/Textbox.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <cstddef>
#include <string>

int main() {
    GUI::Window wnd { { 500, 500 }, "Checkboxes" };

    GUI::Application app(wnd);

    auto& container1 = app.set_main_widget<GUI::Container>();
    container1.set_background_color(Util::Colors::white);
    container1.set_layout<GUI::VerticalBoxLayout>();
    
    auto check1 = container1.add_widget<GUI::Checkbox>();
    check1->set_caption("Sample checkbox");
    check1->set_size({Length::Auto, 15.0_px});
    check1->set_background_color(Util::Colors::white);
    check1->set_foreground_color(Util::Colors::black);
    check1->set_text_color(Util::Colors::black);

    check1->on_change = [](bool state){
        if(state)
            std::cout << "CHECKED\n";
        else
            std::cout << "UNCHECKED\n";
    };

    app.run();
    return 0;
}
