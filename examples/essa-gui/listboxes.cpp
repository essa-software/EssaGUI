#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Checkbox.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Listbox.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
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
    auto& host_window = app.create_host_window({ 500, 500 }, "ListBoxes");

    auto& container1 = host_window.set_root_widget<GUI::Container>();
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
    listbox1->on_change = [](size_t index, bool state) {
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

    app.run();
    return 0;
}
