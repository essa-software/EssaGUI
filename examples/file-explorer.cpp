#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/FileExplorer.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(1000, 1000), "File explorer", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };

    GUI::Application app(wnd);

    auto& file_explorer = app.open_overlay<GUI::FileExplorer>();
    file_explorer.set_size({ 1000, 500 });
    file_explorer.center_on_screen();
    file_explorer.open_path("/");
    file_explorer.run();

    app.run();
    return 0;
}
