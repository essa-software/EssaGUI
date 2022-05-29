#include "EssaGUI/gfx/ResourceLoader.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Console.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/ImageWidget.hpp>
#include <EssaGUI/gui/ToolWindow.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(1000, 1000), "ImageWidget", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };

    GUI::Application app(wnd);

    auto& container1 = app.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::HorizontalBoxLayout>();
    auto image_widget = container1.add_widget<GUI::ImageWidget>();

    sf::Texture texture = Gfx::require_texture("../assets/gui/directory.png");
    image_widget->set_image(&texture);

    app.run();
    return 0;
}
