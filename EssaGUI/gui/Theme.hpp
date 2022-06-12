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

    void load_ini(const std::string path);

    struct BgTextColors {
        sf::Color background {};
        sf::Color foreground {};
        sf::Color text {};
    };

    struct ButtonColors {
        BgTextColors untoggleable;
        BgTextColors active;
        BgTextColors inactive;
    };

    ButtonColors text_button;
    ButtonColors image_button;
    ButtonColors tab_button;

    sf::Color positive; // "green" / the "good" thing like applying changes
    sf::Color negative; // "red" / the "bad" thing like removing objects
    sf::Color neutral;  // "blue"

    BgTextColors slider;

    BgTextColors textfield;

    BgTextColors textbox;
    BgTextColors active_textbox;

    BgTextColors gutter;
    BgTextColors container;

    BgTextColors datebox;
    BgTextColors datebox_active_calendar_day;
    BgTextColors datebox_inactive_calendar_day;

    BgTextColors list_record1;
    BgTextColors list_record2;

    BgTextColors prompt;
    BgTextColors tooltip;

    sf::Color active_selection;
    sf::Color selection;

    sf::Color placeholder;
};

}
