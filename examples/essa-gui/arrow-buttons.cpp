#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/ArrowButton.hpp>
#include <Essa/GUI/Widgets/Container.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Arrow buttons");

    auto& container1 = host_window.set_root_widget<GUI::Container>();
    container1.set_background_color(Util::Colors::White);
    container1.set_layout<GUI::VerticalBoxLayout>();

    auto a1 = container1.add_widget<GUI::ArrowButton>();
    a1->set_arrow_type(GUI::ArrowButton::ArrowType::BOTTOMARROW);
    auto a2 = container1.add_widget<GUI::ArrowButton>();
    a2->set_arrow_type(GUI::ArrowButton::ArrowType::LEFTARROW);
    auto a3 = container1.add_widget<GUI::ArrowButton>();
    a3->set_arrow_type(GUI::ArrowButton::ArrowType::RIGHTARROW);
    auto a4 = container1.add_widget<GUI::ArrowButton>();
    a4->set_arrow_type(GUI::ArrowButton::ArrowType::TOPARROW);

    app.run();
    return 0;
}
