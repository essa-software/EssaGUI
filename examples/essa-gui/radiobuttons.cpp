#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/RadioButton.hpp>
#include <EssaGUI/gui/RadioGroup.hpp>
#include <EssaGUI/gui/TextEditor.hpp>
#include <EssaGUI/gui/Textbox.hpp>
#include <EssaGUI/gui/Widget.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <cstddef>
#include <string>

int main() {
    GUI::Window wnd { { 500, 500 }, "Radiobuttons" };

    GUI::Application app(wnd);

    auto& container1 = app.host_window().set_main_widget<GUI::Container>();
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
