#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/FileExplorer.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(1000, 1000), "File explorer", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };
    
    sf::Font font;
    if(!font.loadFromFile("../assets/fonts/SourceCodePro-Regular.otf"))
        return 1;

    while(true) {
        sf::Event e;
        while(wnd.pollEvent(e)) {
            if(e.type == sf::Event::Closed)
                return 0;
        }
        wnd.clear(Util::Color(100, 100, 100));
        wnd.draw_text("/home/sppmacd #T3$%abcdefghijklmnopqurtsdhstrhstyjstr6", font, {50, 200}, {.font_size = 100});
        wnd.display();
    }

    return 0;
}
