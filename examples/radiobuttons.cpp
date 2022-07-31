#include "EssaGUI/gui/RadioButton.hpp"
#include "EssaGUI/gui/Widget.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/TextEditor.hpp>
#include <EssaGUI/gui/Textbox.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Units.hpp>
#include <string>

int main() {
    GUI::Window wnd { { 500, 500 }, "TextEditor" };

    GUI::Application app(wnd);

    auto& container1 = app.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();
    
    auto radio1 = container1.add_widget<GUI::RadioButton>();
    radio1->set_caption("Radio1");
    radio1->set_size({Length::Auto, 15.0_px});
    radio1->set_toggleable(true);
    radio1->set_text_color(Util::Colors::white);

    radio1->on_change = [](bool change){
        if(change)
            std::cout << "ACTIVE\n";
        else
            std::cout << "INACTIVE\n";
    };

    app.run();
    return 0;
}
