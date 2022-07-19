#include "Theme.hpp"
#include <EssaUtil/Color.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace GUI {

Theme& Theme::default_theme() {
    static Theme th = []() {
        Theme theme;
        theme.text_button = {
            .untoggleable = { .background = Util::Color { 120, 120, 120 }, .text = Util::Colors::white },
            .active = { .background = Util::Color { 80, 200, 80 }, .text = Util::Colors::white },
            .inactive = { .background = Util::Color { 200, 80, 90 }, .text = Util::Colors::white },
        };
        theme.image_button = {
            .untoggleable = { .background = Util::Color { 92, 89, 89 }, .text = Util::Colors::white },
            .active = { .background = Util::Color { 0, 80, 255 }, .text = Util::Colors::white },
            .inactive = { .background = Util::Color { 92, 89, 89 }, .text = Util::Colors::white },
        };
        theme.tab_button = {
            .untoggleable = {},
            .active = { .background = Util::Color { 200, 200, 200, 100 }, .text = Util::Colors::white },
            .inactive = { .background = Util::Color { 120, 120, 120, 100 }, .text = Util::Color { 180, 180, 180 } }
        };
        theme.textbox = {
            .background = Util::Color { 220, 220, 220 },
            .foreground = Util::Color { 120, 120, 120 },
            .text = Util::Color { 30, 30, 30 },
        };
        theme.active_textbox = {
            .background = Util::Color { 220, 220, 220 },
            .foreground = Util::Color { 120, 120, 120 },
            .text = Util::Color { 30, 30, 30 },
        };
        theme.gutter = {
            .background = Util::Color { 200, 200, 200 },
            .foreground = Util::Color { 120, 120, 120 },
            .text = Util::Color { 30, 30, 30 },
        };
        theme.slider = {
            .background = Util::Color { 180, 180, 180 },
            .foreground = Util::Color { 140, 140, 140 },
            .text = Util::Color { 30, 30, 30 },
        };
        theme.tooltip = {
            .background = Util::Color { 100, 100, 100 },
            .foreground = Util::Color { 244, 244, 244 },
            .text = Util::Color { 244, 244, 244 },
        };
        theme.menu = {
            .background = Util::Color { 80, 80, 80 },
            .foreground = Util::Color { 178, 178, 178 },
            .text = Util::Color { 244, 244, 244 },
        };
        theme.positive = Util::Color { 100, 200, 100 };
        theme.negative = Util::Color { 200, 100, 100 };
        theme.neutral = Util::Color { 100, 100, 200 };
        theme.active_selection = Util::Color { 146, 180, 219 };
        theme.selection = Util::Color { 160, 160, 160 };
        theme.placeholder = Util::Color { 160, 160, 160 };
        return theme;
    }();
    return th;
}

std::pair<std::string, Util::Color> read_value(std::string line) {
    std::string key = "", color_str = "";
    bool in_value = 0;
    for (const auto& c : line) {
        if (c == ' ' || c == '\t')
            continue;
        else if (c == '=') {
            in_value = true;
            continue;
        }
        else if (c == '#')
            break;

        if (in_value)
            color_str += c;
        else
            key += c;
    }

    Util::Color color;
    uint8_t color_arr[4] { 0 };
    size_t i = 0;

    for (const auto& c : color_str) {
        if (c == ',') {
            i++;

            if (c == 4)
                break;
            continue;
        }

        if (c >= '0' && c <= '9')
            color_arr[i] = color_arr[i] * 10 + (c - 48);
    }
    color.r = color_arr[0];
    color.g = color_arr[1];
    color.b = color_arr[2];
    color.a = color_arr[3];

    return std::make_pair(key, color);
}

void Theme::load_ini(std::string const& path) {
    const std::map<std::string, Util::Color*> values = {
        // text button
        { "text_button.untoggleable.background_color", &text_button.untoggleable.background },
        { "text_button.untoggleable.foreground_color", &text_button.untoggleable.foreground },
        { "text_button.untoggleable.text_color", &text_button.untoggleable.text },

        { "text_button.active.background_color", &text_button.active.background },
        { "text_button.active.foreground_color", &text_button.active.foreground },
        { "text_button.active.text_color", &text_button.active.text },

        { "text_button.inactive.background_color", &text_button.inactive.background },
        { "text_button.inactive.foreground_color", &text_button.inactive.foreground },
        { "text_button.inactive.text_color", &text_button.inactive.text },

        // image button
        { "image_button.untoggleable.background_color", &image_button.untoggleable.background },
        { "image_button.untoggleable.foreground_color", &image_button.untoggleable.foreground },
        { "image_button.untoggleable.text_color", &image_button.untoggleable.text },

        { "image_button.active.background_color", &image_button.active.background },
        { "image_button.active.foreground_color", &image_button.active.foreground },
        { "image_button.active.text_color", &image_button.active.text },

        { "image_button.inactive.background_color", &image_button.inactive.background },
        { "image_button.inactive.foreground_color", &image_button.inactive.foreground },
        { "image_button.inactive.text_color", &image_button.inactive.text },

        // tab button
        { "tab_button.untoggleable.background_color", &tab_button.untoggleable.background },
        { "tab_button.untoggleable.foreground_color", &tab_button.untoggleable.foreground },
        { "tab_button.untoggleable.text_color", &tab_button.untoggleable.text },

        { "tab_button.active.background_color", &tab_button.active.background },
        { "tab_button.active.foreground_color", &tab_button.active.foreground },
        { "tab_button.active.text_color", &tab_button.active.text },

        { "tab_button.inactive.background_color", &tab_button.inactive.background },
        { "tab_button.inactive.foreground_color", &tab_button.inactive.foreground },
        { "tab_button.inactive.text_color", &tab_button.inactive.text },

        // widget types
        { "widget.positive", &positive },
        { "widget.negative", &negative },
        { "widget.neutral", &neutral },

        // slider
        { "slider.background_color", &slider.background },
        { "slider.foreground_color", &slider.foreground },
        { "slider.text_color", &slider.text },

        // textfield
        { "textfield.background_color", &textfield.background },
        { "textfield.foreground_color", &textfield.foreground },
        { "textfield.text_color", &textfield.text },

        // textbox
        { "textbox.background_color", &textbox.background },
        { "textbox.foreground_color", &textbox.foreground },
        { "textbox.text_color", &textbox.text },

        { "textbox_focused.background_color", &active_textbox.background },
        { "textbox_focused.foreground_color", &active_textbox.foreground },
        { "textbox_focused.text_color", &active_textbox.text },

        { "gutter.background_color", &gutter.background },
        { "gutter.foreground_color", &gutter.foreground },
        { "gutter.text_color", &gutter.text },

        // datebox
        { "datebox.background_color", &datebox.background },
        { "datebox.foreground_color", &datebox.foreground },
        { "datebox.text_color", &datebox.text },

        { "datebox_active_calendar_day.background_color", &datebox_active_calendar_day.background },
        { "datebox_active_calendar_day.foreground_color", &datebox_active_calendar_day.foreground },
        { "datebox_active_calendar_day.text_color", &datebox_active_calendar_day.text },

        { "datebox_inactive_calendar_day.background_color", &datebox_inactive_calendar_day.background },
        { "datebox_inactive_calendar_day.foreground_color", &datebox_inactive_calendar_day.foreground },
        { "datebox_inactive_calendar_day.text_color", &datebox_inactive_calendar_day.text },

        // Widget list
        { "widget_list_odd.background_color", &list_record1.background },
        { "widget_list_odd.foreground_color", &list_record1.foreground },
        { "widget_list_odd.text_color", &list_record1.text },

        { "widget_list_even.background_color", &list_record2.background },
        { "widget_list_even.foreground_color", &list_record2.foreground },
        { "widget_list_even.text_color", &list_record2.text },

        // Prompts
        { "prompt.background_color", &prompt.background },
        { "prompt.foreground_color", &prompt.foreground },
        { "prompt.text_color", &prompt.text },

        // Tooltips
        { "tooltip.background_color", &tooltip.background },
        { "tooltip.foreground_color", &tooltip.foreground },
        { "tooltip.text_color", &tooltip.text },

        // Menu
        { "menu.background_color", &menu.background },
        { "menu.foreground_color", &menu.foreground },
        { "menu.text_color", &menu.text },

        // Selection etc.
        { "active_selection", &active_selection },
        { "selection", &selection },
        { "placeholder", &placeholder },
    };
    std::ifstream f_in(path);

    while (!f_in.eof()) {
        std::string line;
        std::getline(f_in, line);

        auto val = read_value(line);

        if (val.first.size() == 0)
            continue;

        auto it = values.find(val.first);

        if (it != values.end()) {
            (*it->second) = val.second;
        }
        else {
            std::cout << "Unknown attribute "
                         "\""
                      << val.first << "\'!\n";
        }
    }
}

}
