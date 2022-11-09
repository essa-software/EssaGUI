#pragma once

#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/RadioButton.hpp>
#include <EssaGUI/gui/Widget.hpp>
#include <EssaUtil/UString.hpp>
#include <functional>
#include <vector>

namespace GUI {

class RadioGroup : public Container {
public:
    virtual void on_init() override;

    void add_radio(Util::UString const& caption);
    void set_index(size_t index);
    size_t get_index() const { return m_index; }
    virtual void update() override;

    CREATE_VALUE(Util::Length, row_height, 15.0_px)

    std::function<void(size_t)> on_change;

private:
    size_t m_index = 0;

    std::vector<RadioButton*> m_buttons;

protected:
    using Container::add_widget;
    using Container::add_created_widget;
    using Container::clear_layout;
    using Container::get_layout;
    using Container::shrink;
    using Container::widgets;
};

}
