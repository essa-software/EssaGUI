
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Grid.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 800, 600 }, "Grids");

    auto& container = host_window.set_main_widget<GUI::Container>();
    container.set_layout<GUI::HorizontalBoxLayout>();

    auto grid = container.add_widget<GUI::Grid>();
    grid->resize(3, 3);

    auto cell = grid->get_cell(0, 0);
    auto child = cell->add_child<GUI::Textfield>();
    child->set_background_color(Util::Colors::AliceBlue);

    cell = grid->get_cell(1, 1);
    child = cell->add_child<GUI::Textfield>();
    child->set_background_color(Util::Colors::BlueViolet);

    app.run();
}
