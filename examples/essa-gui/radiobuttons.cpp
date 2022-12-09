#include <Essa/GUI/Application.hpp>
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
    auto& host_window = app.create_host_window({ 500, 500 }, "Radio buttons");

    auto& container1 = host_window.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();

    auto radiogrp = container1.add_widget<GUI::RadioGroup>();

    for (size_t i = 0; i < 5; i++) {
        radiogrp->add_radio("radio" + Util::to_ustring(i));
    }

    radiogrp->on_change = [](size_t index) {
        std::cout << "Selected index: " << index << "\n";
    };

    radiogrp->set_row_height(15.0_px);

    app.run();
    return 0;
}
