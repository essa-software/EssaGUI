#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/ScrollableContainer.hpp>
#include <Essa/GUI/Widgets/ScrollableWidget.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>

int main() {
    GUI::SimpleApplication<GUI::Container> app { "ScrollableContainer", { 1000, 500 } };

    app.main_widget().set_layout<GUI::HorizontalBoxLayout>();
    auto& vertical = app.main_widget().add_widget<GUI::ScrollableContainer>()->set_widget<GUI::Container>();
    auto& horizontal = app.main_widget().add_widget<GUI::ScrollableContainer>()->set_widget<GUI::Container>();

    {
        auto& layout = horizontal.set_layout<GUI::HorizontalBoxLayout>();
        layout.set_padding(GUI::Boxi::all_equal(10));

        for (int i = 0; i < 10; i++) {
            auto* button = horizontal.add_widget<GUI::TextButton>();
            button->set_content(Util::to_ustring(i));
            button->set_size({ 50.0_px, Util::Length::Auto });
        }
    }

    {
        auto& layout = vertical.set_layout<GUI::VerticalBoxLayout>();
        layout.set_padding(GUI::Boxi::all_equal(10));

        for (int i = 0; i < 10; i++) {
            auto* button = vertical.add_widget<GUI::TextButton>();
            button->set_content(Util::to_ustring(i));
            button->set_size({ Util::Length::Auto, 50.0_px });
        }
    }
    app.run();
    return 0;
}
