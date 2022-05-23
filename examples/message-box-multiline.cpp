#include <EssaGUI/gfx/SFMLWindow.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(1000, 1000), "File explorer", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };
    GUI::Application app(wnd);
    GUI::message_box("Test\nmultiline\nAND VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY LONG LINE\nmessage", "test", GUI::MessageBox::Buttons::Ok);
    return 0;
}
