#include <EssaGUI/Application.hpp>
#include <EssaGUI/Overlays/MessageBox.hpp>
#include <EssaGUI/Sprites/RectangleShape.hpp>
#include <EssaGUI/TextAlign.hpp>
#include <EssaGUI/Widgets/Canvas.hpp>
#include <EssaGUI/Widgets/Checkbox.hpp>
#include <EssaGUI/Widgets/Container.hpp>
#include <EssaGUI/Widgets/Listbox.hpp>
#include <EssaGUI/Widgets/RadioButton.hpp>
#include <EssaGUI/Widgets/RadioGroup.hpp>
#include <EssaGUI/Widgets/TextButton.hpp>
#include <EssaGUI/Widgets/TextEditor.hpp>
#include <EssaGUI/Widgets/Textbox.hpp>
#include <EssaGUI/Widgets/Widget.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <EssaUtil/Vector.hpp>
#include <cstddef>
#include <string>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 800, 600 }, "Canvas");

    auto& container1 = host_window.set_main_widget<GUI::Container>();
    container1.set_background_color(Util::Colors::White);
    container1.set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);

    auto container2 = container1.add_widget<GUI::Container>();
    container2->set_layout<GUI::VerticalBoxLayout>().set_spacing(10);
    container2->set_size({ 250.0_px, Util::Length::Auto });

    auto btn1 = container2->add_widget<GUI::TextButton>();
    btn1->set_alignment(GUI::Align::Center);
    btn1->set_content("Move");
    btn1->set_size({ 200.0_px, 15.0_px });

    auto btn2 = container2->add_widget<GUI::TextButton>();
    btn2->set_alignment(GUI::Align::Center);
    btn2->set_content("Rotate");
    btn2->set_size({ 200.0_px, 15.0_px });

    auto canvas = container1.add_widget<GUI::Canvas>();
    canvas->set_background_color(Util::Colors::Gray);

    auto rectangle = canvas->add_sprite<GUI::RectangleShape>();
    rectangle->set_background_color(Util::Colors::Blue);
    rectangle->set_foreground_color(Util::Colors::Orange);
    rectangle->set_position({ 50.0_px, 50.0_px });
    rectangle->set_size({ 30.0_px, 20.0_px });

    btn1->on_click = [&]() {
        rectangle->move_by_vec(Util::Vector2f(25, 20));
    };

    app.run();
    return 0;
}
