#include "Theme.hpp"

#include <EssaGUI/util/ConfigFile.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Error.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace GUI {

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

Util::OsErrorOr<void> Theme::load_ini(std::string const& path) {
    std::map<std::string, Util::Color*> const theme_options = {
        // text button
        { "text_button.untoggleable.background", &text_button.untoggleable.background },
        { "text_button.untoggleable.foreground", &text_button.untoggleable.foreground },
        { "text_button.untoggleable.text", &text_button.untoggleable.text },

        { "text_button.active.background", &text_button.active.background },
        { "text_button.active.foreground", &text_button.active.foreground },
        { "text_button.active.text", &text_button.active.text },

        { "text_button.inactive.background", &text_button.inactive.background },
        { "text_button.inactive.foreground", &text_button.inactive.foreground },
        { "text_button.inactive.text", &text_button.inactive.text },

        // image button
        { "image_button.untoggleable.background", &image_button.untoggleable.background },
        { "image_button.untoggleable.foreground", &image_button.untoggleable.foreground },
        { "image_button.untoggleable.text", &image_button.untoggleable.text },

        { "image_button.active.background", &image_button.active.background },
        { "image_button.active.foreground", &image_button.active.foreground },
        { "image_button.active.text", &image_button.active.text },

        { "image_button.inactive.background", &image_button.inactive.background },
        { "image_button.inactive.foreground", &image_button.inactive.foreground },
        { "image_button.inactive.text", &image_button.inactive.text },

        // tab button
        { "tab_button.untoggleable.background", &tab_button.untoggleable.background },
        { "tab_button.untoggleable.foreground", &tab_button.untoggleable.foreground },
        { "tab_button.untoggleable.text", &tab_button.untoggleable.text },

        { "tab_button.active.background", &tab_button.active.background },
        { "tab_button.active.foreground", &tab_button.active.foreground },
        { "tab_button.active.text", &tab_button.active.text },

        { "tab_button.inactive.background", &tab_button.inactive.background },
        { "tab_button.inactive.foreground", &tab_button.inactive.foreground },
        { "tab_button.inactive.text", &tab_button.inactive.text },

        // widget types
        { "positive", &positive },
        { "negative", &negative },
        { "neutral", &neutral },

        // slider
        { "slider.background", &slider.background },
        { "slider.foreground", &slider.foreground },
        { "slider.text", &slider.text },

        // textfield
        { "textfield.background", &textfield.background },
        { "textfield.foreground", &textfield.foreground },
        { "textfield.text", &textfield.text },

        // textbox
        { "textbox.background", &textbox.background },
        { "textbox.foreground", &textbox.foreground },
        { "textbox.text", &textbox.text },

        { "active_textbox.background", &active_textbox.background },
        { "active_textbox.foreground", &active_textbox.foreground },
        { "active_textbox.text", &active_textbox.text },

        { "gutter.background", &gutter.background },
        { "gutter.foreground", &gutter.foreground },
        { "gutter.text", &gutter.text },

        // datebox
        { "datebox.background", &datebox.background },
        { "datebox.foreground", &datebox.foreground },
        { "datebox.text", &datebox.text },

        { "datebox_active_calendar_day.background", &datebox_active_calendar_day.background },
        { "datebox_active_calendar_day.foreground", &datebox_active_calendar_day.foreground },
        { "datebox_active_calendar_day.text", &datebox_active_calendar_day.text },

        { "datebox_inactive_calendar_day.background", &datebox_inactive_calendar_day.background },
        { "datebox_inactive_calendar_day.foreground", &datebox_inactive_calendar_day.foreground },
        { "datebox_inactive_calendar_day.text", &datebox_inactive_calendar_day.text },

        // Widget list
        { "list_record1.background", &list_record1.background },
        { "list_record1.foreground", &list_record1.foreground },
        { "list_record1.text", &list_record1.text },

        { "list_record2.background", &list_record2.background },
        { "list_record2.foreground", &list_record2.foreground },
        { "list_record2.text", &list_record2.text },

        // Prompts
        { "prompt.background", &prompt.background },
        { "prompt.foreground", &prompt.foreground },
        { "prompt.text", &prompt.text },

        // Tooltips
        { "tooltip.background", &tooltip.background },
        { "tooltip.foreground", &tooltip.foreground },
        { "tooltip.text", &tooltip.text },

        // Menu
        { "menu.background", &menu.background },
        { "menu.foreground", &menu.foreground },
        { "menu.text", &menu.text },

        // Selection etc.
        { "active_selection", &active_selection },
        { "selection", &selection },
        { "placeholder", &placeholder },
    };

    auto theme_ini_file = TRY(Util::ConfigFile::open_ini(path));

    for (auto const& keys : theme_options) {
        auto color = theme_ini_file.get_color(keys.first);
        if (!color) {
            std::cout << "Theme: Missing value for " << keys.first << std::endl;
            *keys.second = Util::Colors::magenta; // So that you easily see the bug
        }
        else {
            *keys.second = *color;
        }
    }

    return {};
}

}
