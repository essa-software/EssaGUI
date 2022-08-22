#include "Theme.hpp"

#include "Button.hpp"
#include "Widget.hpp"

#include <EssaGUI/util/ConfigFile.hpp>
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

static void add_theme_option(std::map<std::string, Util::Color*>& options, std::string key, Util::Color& var) {
    std::cout << key << std::endl;
    options.insert({ key, &var });
}

static void add_theme_option(std::map<std::string, Util::Color*>& options, std::string key, Theme::BgFgTextColors& var) {
    add_theme_option(options, key + ".background", var.background);
    add_theme_option(options, key + ".foreground", var.foreground);
    add_theme_option(options, key + ".text", var.text);
}

static void add_theme_option(std::map<std::string, Util::Color*>& options, std::string key, Theme::TextboxColors& var) {
    add_theme_option(options, key + ".disabled", var.disabled);
    add_theme_option(options, key + ".normal", var.normal);
}

static void add_theme_option(std::map<std::string, Util::Color*>& options, std::string key, Theme::SelectionColors& var) {
    add_theme_option(options, key + ".focused", var.focused);
    add_theme_option(options, key + ".unfocused", var.unfocused);
}

static void add_theme_option(std::map<std::string, Util::Color*>& options, std::string key, Theme::HoverableWidgetColors& var) {
    add_theme_option(options, key + ".hovered", var.hovered);
    add_theme_option(options, key + ".unhovered", var.unhovered);
}

static void add_theme_option(std::map<std::string, Util::Color*>& options, std::string key, Theme::ButtonColors& var) {
    add_theme_option(options, key + ".active", var.active);
    add_theme_option(options, key + ".inactive", var.inactive);
    add_theme_option(options, key + ".normal", var.normal);
    add_theme_option(options, key + ".disabled", var.disabled);
}

Util::OsErrorOr<void> Theme::load_ini(std::string const& path) {
    auto const theme_options = [this]() {
        std::map<std::string, Util::Color*> options;

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

#undef DEFINE_THEME_OPTION

        return options;
    }();

    auto theme_ini_file
        = TRY(Util::ConfigFile::open_ini(path));

    for (auto const& keys : theme_options) {
        auto color = theme_ini_file.get_color(keys.first);
        if (!color) {
            std::cout << "Theme: Missing value for " << keys.first << std::endl;
            *keys.second = Util::Colors::Magenta; // So that you easily see the bug
        }
        else {
            *keys.second = *color;
        }
    }

    return {};
}

}
