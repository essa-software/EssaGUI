#include "SettingsMenu.hpp"

#include "Container.hpp"
#include "Frame.hpp"

#include <iostream>

namespace GUI {

SettingsMenu::SettingsMenu(Container& c)
    : Container(c) {
    auto& layout = set_layout<HorizontalBoxLayout>();
    layout.set_spacing(10);
    m_buttons_container = add_widget<Container>();
    {
        m_buttons_container->set_size({ 72.0_px, Length::Auto });
        auto& buttons_container_layout = m_buttons_container->set_layout<VerticalBoxLayout>();
        buttons_container_layout.set_spacing(10);
    }
    m_settings_container = add_widget<Container>();
    m_settings_container->set_layout<BasicLayout>();
}

SettingsMenu::MenuEntry& SettingsMenu::add_entry(sf::Image const& image, std::string tooltip, Expandable expandable) {
    auto button = m_buttons_container->add_widget<ImageButton>(image);
    button->set_tooltip_text(tooltip);
    if (expandable == Expandable::Yes) {
        button->set_toggleable(true);
        button->on_change = [this, button](bool state) {
            for (auto& entry : m_entries) {
                if (!entry->settings_container)
                    continue;
                if (entry->button == button) {
                    entry->settings_container->set_visible(state);
                    if (entry->on_toggle)
                        entry->on_toggle(state);
                    entry->button->set_active(state, NotifyUser::No);
                }
                else {
                    entry->settings_container->set_visible(false);
                    if (entry->button->is_active() && entry->on_toggle)
                        entry->on_toggle(false);
                    entry->button->set_active(false, NotifyUser::No);
                }
            }
        };
        auto settings_container = m_settings_container->add_widget<Frame>();
        settings_container->set_visible(false);
        return *m_entries.emplace_back(std::make_unique<MenuEntry>(button, settings_container));
    }
    auto entry = m_entries.emplace_back(std::make_unique<MenuEntry>(button, nullptr)).get();
    button->on_click = [entry]() {
        entry->on_toggle(true);
    };
    return *m_entries.back();
}

}
