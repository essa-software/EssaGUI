
#include "EssaGUI/gui/Application.hpp"
#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/Grid.hpp"
#include "EssaGUI/gui/Textfield.hpp"
int main(){
    GUI::Window wnd { { 800, 600 }, "Listboxes" };
    GUI::Application app(wnd);

    auto& container = app.set_main_widget<GUI::Container>();
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

