#pragma once

#include <SFML/Graphics.hpp>
#include <optional>

namespace GUI {

class Theme {
public:
    Theme() = default;
    Theme(Theme const&) = delete;
    Theme& operator=(Theme const&) = delete;
    Theme(Theme&&) = default;
    Theme& operator=(Theme&&) = default;

    static Theme& default_theme();

    struct BgTextColors {
        sf::Color background;
        sf::Color text;
    };

    struct ButtonColors {
        BgTextColors untoggleable;
        BgTextColors active;
        BgTextColors inactive;
    };

    ButtonColors text_button;
    ButtonColors image_button;
    ButtonColors tab_button;
};

}
