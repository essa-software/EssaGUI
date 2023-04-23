#include "Essa/GUI/Widgets/Container.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/ScrollableContainer.hpp>
#include <Essa/GUI/Widgets/ScrollableWidget.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>

int main() {
    GUI::SimpleApplication<GUI::ScrollableContainer> app { "ScrollableContainer" };

    auto& widget = app.main_widget();
    auto& container = widget.set_widget<GUI::Container>();
    auto& layout = container.set_layout<GUI::VerticalBoxLayout>();
    layout.set_padding(GUI::Boxi::all_equal(10));

    auto add_button = [&](int idx) {
        auto button = container.add_widget<GUI::TextButton>();
        button->set_content(Util::to_ustring(idx));
        button->set_size({ Util::Length::Auto, 50.0_px });
    };

    for (int i = 0; i < 10; i++) {
        add_button(i);
    }
    app.run();
    return 0;
}
