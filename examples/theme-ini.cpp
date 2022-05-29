#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/ToolWindow.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Console.hpp>
#include <EssaGUI/gui/Theme.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(1000, 1000), "File explorer", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };
        
    GUI::Theme theme;
    theme.load_ini("../examples/test_ini.ini");
    return 0;
}
