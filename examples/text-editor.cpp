#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/TextEditor.hpp>
#include <SFML/Graphics.hpp>

int main() {
    GUI::SFMLWindow wnd { sf::VideoMode(1000, 1000), "TextEditor", sf::Style::Default,
        sf::ContextSettings { 0, 0, 0, 3, 2 } };

    GUI::Application app(wnd);

    auto& container1 = app.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::HorizontalBoxLayout>();
    auto image_widget = container1.add_widget<GUI::TextEditor>();

    app.run();
    return 0;
}
