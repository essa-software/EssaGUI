#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/ArrowButton.hpp>
#include <EssaGUI/gui/Container.hpp>

int main() {
    GUI::Window wnd { { 500, 500 }, "Checkboxes" };

    GUI::Application app(wnd);

    auto& container1 = app.host_window().set_main_widget<GUI::Container>();
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
