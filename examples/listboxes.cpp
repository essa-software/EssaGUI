#include "EssaGUI/gui/Checkbox.hpp"
#include "EssaGUI/gui/Listbox.hpp"
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
    GUI::Window wnd { { 500, 500 }, "Listboxes" };

    GUI::Application app(wnd);

    auto& container1 = app.set_main_widget<GUI::Container>();
    container1.set_background_color(Util::Colors::White);
    container1.set_layout<GUI::HorizontalBoxLayout>();

    auto listbox1 = container1.add_widget<GUI::Listbox>();
    listbox1->add("d");
    listbox1->add("b");
    listbox1->add("c");
    listbox1->add("a");
    listbox1->set_row_height(15.0_px);
    listbox1->allow_multichoose(false);
    listbox1->sorted_list(true);
    listbox1->set_background_color(Util::Colors::White);
    listbox1->set_foreground_color(Util::Colors::Blue);
    listbox1->set_text_color(Util::Colors::Black);
    listbox1->on_change = [](size_t index, bool state){
        std::cout << "Element " << index << " is now " << (state ? "enabled" : "disabled") << std::endl;
    };

    auto listbox2 = container1.add_widget<GUI::Listbox>();
    listbox2->add("d");
    listbox2->add("b");
    listbox2->add("c");
    listbox2->add("a");
    listbox2->set_row_height(15.0_px);
    listbox2->allow_multichoose(true);
    listbox2->sorted_list(false);
    listbox2->set_background_color(Util::Colors::White);
    listbox2->set_foreground_color(Util::Colors::Green);
    listbox2->set_text_color(Util::Colors::Black);

    app.run();
    return 0;
}
