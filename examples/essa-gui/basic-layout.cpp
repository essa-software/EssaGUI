#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/TextButton.hpp>

int main() {
    GUI::Application app;
    auto& window = app.create_host_window({ 500, 500 }, "Basic Layout");

    auto& container = window.set_main_widget<GUI::Container>();
    container.set_layout<GUI::BasicLayout>();

    auto top_left = container.add_widget<GUI::TextButton>();
    top_left->set_content("Top left");
    top_left->set_position({ 10.0_px, 10.0_px });
    top_left->set_size({ 150.0_px, 32.0_px });

    auto center = container.add_widget<GUI::TextButton>();
    center->set_alignments(GUI::Widget::Alignment::Center, GUI::Widget::Alignment::Center);
    center->set_content("Center");
    center->set_size({ 150.0_px, 32.0_px });

    auto bottom_right = container.add_widget<GUI::TextButton>();
    bottom_right->set_alignments(GUI::Widget::Alignment::End, GUI::Widget::Alignment::End);
    bottom_right->set_content("Bottom right");
    bottom_right->set_position({ 10.0_px, 10.0_px });
    bottom_right->set_size({ 150.0_px, 32.0_px });

    app.run();
    return 0;
}
