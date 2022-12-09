#pragma once

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/Error.hpp>
#include <optional>
#include <string>

namespace GUI {

class Button;
class Widget;

class Theme {
public:
    Theme() = default;
    Theme(Theme const&) = delete;
    Theme& operator=(Theme const&) = delete;
    Theme(Theme&&) = default;
    Theme& operator=(Theme&&) = default;

    static Theme& default_theme();

    Util::OsErrorOr<void> load_ini(std::string const& path);

    struct BgFgTextColors {
        Util::Color background {};
        Util::Color foreground {};
        Util::Color text {};
    };

    struct HoverableWidgetColors {
        BgFgTextColors hovered;
        BgFgTextColors unhovered;

        BgFgTextColors value(Widget const& w) const;

        void set_colors(Util::Color const& color) {
            unhovered = { .background = color, .foreground = {}, .text = Util::Colors::White };
            hovered = { .background = color + Util::Color { 20, 20, 20 }, .foreground = {}, .text = Util::Colors::White };
        }
    };

    struct ButtonColors {
        HoverableWidgetColors active;
        HoverableWidgetColors inactive;
        HoverableWidgetColors normal;
        BgFgTextColors disabled;

        BgFgTextColors value(Button const& w) const;
    };

    struct TextboxColors {
        BgFgTextColors normal;
        BgFgTextColors disabled;

        BgFgTextColors value(Widget const& w) const;
    };

    struct SelectionColors {
        Util::Color focused;
        Util::Color unfocused;

        Util::Color value(Widget const& w) const;
    };

    ButtonColors image_button;
    ButtonColors text_button;
    ButtonColors tab_button;

    TextboxColors textbox;
    SelectionColors selection;

    BgFgTextColors gutter;
    BgFgTextColors label;
    BgFgTextColors list_even;
    BgFgTextColors list_odd;
    BgFgTextColors menu;
    BgFgTextColors slider;
    BgFgTextColors tooltip;

    Util::Color placeholder;
    Util::Color sidebar;
    Util::Color focus_frame;

    Util::Color positive; // "Green" / the "good" thing like applying changes
    Util::Color negative; // "Red" / the "bad" thing like removing objects
    Util::Color neutral;  // "Blue"

    uint32_t label_font_size;

    uint32_t line_height; // Default height of buttons, textboxes etc.
};

}
