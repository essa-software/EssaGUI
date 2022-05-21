#pragma once

#include "Container.hpp"
#include "ImageButton.hpp"
#include <vector>

namespace GUI {

class SettingsMenu : public Container {
public:
    explicit SettingsMenu(Container& c);

    struct MenuEntry {
        ImageButton* button {};
        Container* settings_container {};
        std::function<void(bool)> on_toggle;

        MenuEntry(ImageButton* button_, Container* container_)
            : button(button_)
            , settings_container(container_) { }
    };

    enum class Expandable {
        Yes,
        No
    };

    MenuEntry& add_entry(sf::Image const& image, std::string tooltip, Expandable = Expandable::Yes);

private:
    virtual bool isolated_focus() const override { return true; }

    std::vector<std::unique_ptr<MenuEntry>> m_entries;
    Container* m_buttons_container {};
    Container* m_settings_container {};
};

}
