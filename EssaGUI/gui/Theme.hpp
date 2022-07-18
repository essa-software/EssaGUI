#pragma once

#include <EssaUtil/Color.hpp>
#include <optional>
#include <string>

namespace GUI {

class Theme {
public:
    Theme() = default;
    Theme(Theme const&) = delete;
    Theme& operator=(Theme const&) = delete;
    Theme(Theme&&) = default;
    Theme& operator=(Theme&&) = default;

    static Theme& default_theme();

    void load_ini(std::string const& path);

    struct BgTextColors {
        Util::Color background {};
        Util::Color foreground {};
        Util::Color text {};
    };

    struct ButtonColors {
        BgTextColors untoggleable;
        BgTextColors active;
        BgTextColors inactive;
    };

    ButtonColors text_button;
    ButtonColors image_button;
    ButtonColors tab_button;

    Util::Color positive; // "green" / the "good" thing like applying changes
    Util::Color negative; // "red" / the "bad" thing like removing objects
    Util::Color neutral;  // "blue"

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

    BgTextColors menu;

    Util::Color active_selection;
    Util::Color selection;

    Util::Color placeholder;
};

}
