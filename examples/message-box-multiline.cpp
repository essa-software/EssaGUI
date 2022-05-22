#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow wnd { sf::VideoMode(1000, 1000), "File explorer" };
    GUI::Application app(wnd);
    GUI::message_box("Test\nmultiline\nAND VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY VERY LONG LINE\nmessage", "test", GUI::MessageBox::Buttons::Ok);
    return 0;
}
