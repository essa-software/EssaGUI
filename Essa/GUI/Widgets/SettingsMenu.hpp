#pragma once

#include <Essa/GUI/Widgets/Container.hpp>
#include "ImageButton.hpp"
#include <vector>

namespace GUI {

class SettingsMenu : public Container {
public:
    virtual void on_init() override;

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

    MenuEntry& add_entry(llgl::Texture const& image, Util::UString tooltip, Expandable = Expandable::Yes);

private:
    virtual bool steals_focus() const override { return true; }

    std::vector<std::unique_ptr<MenuEntry>> m_entries;
    Container* m_buttons_container {};
    Container* m_settings_container {};

protected:
    using Container::add_widget;
    using Container::add_created_widget;
    using Container::clear_layout;
    using Container::get_layout;
    using Container::shrink;
    using Container::widgets;
};

}
