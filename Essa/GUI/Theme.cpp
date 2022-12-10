#include "Theme.hpp"

#include <Essa/GUI/Util/ConfigFile.hpp>
#include <Essa/GUI/Widgets/Button.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Error.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace GUI {

Theme::BgFgTextColors Theme::ButtonColors::value(Button const& w) const {
    if (!w.are_all_parents_enabled())
        return disabled;
    if (w.is_toggleable()) {
        if (w.is_active())
            return active.value(w);
        else
            return inactive.value(w);
    }
    return normal.value(w);
}

Theme::BgFgTextColors Theme::HoverableWidgetColors::value(Widget const& w) const {
    if (w.is_hover())
        return hovered;
    return unhovered;
}

Theme::BgFgTextColors Theme::TextboxColors::value(Widget const& w) const {
    if (w.are_all_parents_enabled())
        return normal;
    return disabled;
}

Util::Color Theme::SelectionColors::value(Widget const& w) const {
    if (w.is_focused())
        return focused;
    return unfocused;
}

Theme& Theme::default_theme() {
    static Theme theme = []() {
        Theme th;
        auto result = th.load_ini("../assets/Theme.ini");
        if (result.is_error()) {
            std::cerr << "Theme: Failed to load default theme: " << strerror(result.error().error) << std::endl;
            abort();
        }
        return th;
    }();
    return theme;
}

using ThemeOption = std::variant<Util::Color*, uint32_t*>;
using ThemeOptionMap = std::map<std::string, ThemeOption>;

template<class T>
requires requires(ThemeOption t) { std::get<T>(t); }
static void add_theme_option(ThemeOptionMap& options, std::string key, T& var) {
    options.insert({ key, &var });
}

static void add_theme_option(ThemeOptionMap& options, std::string key, Theme::BgFgTextColors& var) {
    add_theme_option(options, key + ".background", var.background);
    add_theme_option(options, key + ".foreground", var.foreground);
    add_theme_option(options, key + ".text", var.text);
}

static void add_theme_option(ThemeOptionMap& options, std::string key, Theme::TextboxColors& var) {
    add_theme_option(options, key + ".disabled", var.disabled);
    add_theme_option(options, key + ".normal", var.normal);
}

static void add_theme_option(ThemeOptionMap& options, std::string key, Theme::SelectionColors& var) {
    add_theme_option(options, key + ".focused", var.focused);
    add_theme_option(options, key + ".unfocused", var.unfocused);
}

static void add_theme_option(ThemeOptionMap& options, std::string key, Theme::HoverableWidgetColors& var) {
    add_theme_option(options, key + ".hovered", var.hovered);
    add_theme_option(options, key + ".unhovered", var.unhovered);
}

static void add_theme_option(ThemeOptionMap& options, std::string key, Theme::ButtonColors& var) {
    add_theme_option(options, key + ".active", var.active);
    add_theme_option(options, key + ".inactive", var.inactive);
    add_theme_option(options, key + ".normal", var.normal);
    add_theme_option(options, key + ".disabled", var.disabled);
}

Util::OsErrorOr<void> Theme::load_ini(std::string const& path) {
    auto const theme_options = [this]() {
        ThemeOptionMap options;

#define DEFINE_THEME_OPTION(var) add_theme_option(options, #var, var)

        DEFINE_THEME_OPTION(image_button);
        DEFINE_THEME_OPTION(text_button);
        DEFINE_THEME_OPTION(tab_button);

        DEFINE_THEME_OPTION(textbox);
        DEFINE_THEME_OPTION(selection);

        DEFINE_THEME_OPTION(gutter);
        DEFINE_THEME_OPTION(label);
        DEFINE_THEME_OPTION(list_even);
        DEFINE_THEME_OPTION(list_odd);
        DEFINE_THEME_OPTION(menu);
        DEFINE_THEME_OPTION(slider);
        DEFINE_THEME_OPTION(tooltip);

        DEFINE_THEME_OPTION(placeholder);
        DEFINE_THEME_OPTION(sidebar);
        DEFINE_THEME_OPTION(focus_frame);

        DEFINE_THEME_OPTION(positive); // "Green" / the "good" thing like applying changes
        DEFINE_THEME_OPTION(negative); // "Red" / the "bad" thing like removing objects
        DEFINE_THEME_OPTION(neutral);  // "Blue"

        DEFINE_THEME_OPTION(label_font_size);
        DEFINE_THEME_OPTION(line_height);

#undef DEFINE_THEME_OPTION

        return options;
    }();

    auto theme_ini_file
        = TRY(Util::ConfigFile::open_ini(path));

    for (auto const& keys : theme_options) {
        std::visit([&theme_ini_file, &keys](auto a) {
            if constexpr (std::is_same_v<decltype(a), Util::Color*>) {
                auto color = theme_ini_file.get_color(keys.first);
                if (!color) {
                    std::cout << "Theme: Missing or invalid color value for " << keys.first << std::endl;
                    *a = Util::Colors::Magenta; // So that you easily see the bug
                }
                else {
                    *a = *color;
                }
            }
            else if constexpr (std::is_same_v<decltype(a), uint32_t*>) {
                auto value = theme_ini_file.get_u32(keys.first);
                if (!value) {
                    std::cout << "Theme: Missing or invalid u32 value for " << keys.first << std::endl;
                    *a = 32;
                }
                else {
                    *a = *value;
                }
            }
            else {
                std::cout << "This should be a compile-time error!" << std::endl;
            }
        },
            keys.second);
    }

    return {};
}

}