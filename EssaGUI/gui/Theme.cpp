#include "Theme.hpp"

namespace GUI {

Theme& Theme::default_theme() {
    static Theme th = []() {
        Theme theme;
        theme.text_button = {
            .untoggleable = { .background = sf::Color { 120, 120, 120 }, .text = sf::Color::White },
            .active = { .background = sf::Color { 80, 200, 80 }, .text = sf::Color::White },
            .inactive = { .background = sf::Color { 200, 80, 90 }, .text = sf::Color::White },
        };
        theme.image_button = {
            .untoggleable = { .background = sf::Color { 92, 89, 89 }, .text = sf::Color::White },
            .active = { .background = sf::Color { 0, 80, 255 }, .text = sf::Color::White },
            .inactive = { .background = sf::Color { 92, 89, 89 }, .text = sf::Color::White },
        };
        theme.tab_button = {
            .active = { .background = sf::Color { 200, 200, 200, 100 }, .text = sf::Color::White },
            .inactive = { .background = sf::Color { 80, 80, 80, 100 }, .text = sf::Color { 180, 180, 180 } }
        };
        return theme;
    }();
    return th;
}

}
