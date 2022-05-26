#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/ToolWindow.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Console.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(1000, 1000), "File explorer", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };

    GUI::Application app(wnd);

    auto& wnd1 = app.open_overlay<GUI::ToolWindow>();
    wnd1.set_size({ 500, 100 });
    wnd1.center_on_screen();
    auto& container1 = wnd1.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::HorizontalBoxLayout>();
    auto console = container1.add_widget<GUI::Console>();
    console->append_line({.color = sf::Color::Red, .text = "test1"});
    console->append_line({.color = sf::Color::Yellow, .text = "test2"});
    console->append_line({.color = sf::Color::Green, .text = "test3"});
    console->append_line({.color = sf::Color::Blue, .text = "test4"});
    console->append_line({.color = sf::Color::Red, .text = "test5"});
    console->append_line({.color = sf::Color::Yellow, .text = "test6"});
    console->append_line({.color = sf::Color::Green, .text = "test7"});
    console->append_line({.color = sf::Color::Blue, .text = "test8"});

    app.run();
    return 0;
}
