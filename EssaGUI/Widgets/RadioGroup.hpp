#pragma once

#include <EssaGUI/Widgets/Container.hpp>
#include <EssaGUI/Widgets/RadioButton.hpp>
#include <EssaGUI/Widgets/Widget.hpp>
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

    using Container::find_widgets_by_class_name;
    using Container::find_widgets_by_class_name_recursively;
    using Container::find_widgets_of_type_by_class_name;
    using Container::find_widgets_of_type_by_class_name_recursively;
    using Container::find_widget_by_id;
    using Container::find_widget_by_id_recursively;
    using Container::find_widget_of_type_by_id;
    using Container::find_widget_of_type_by_id_recursively;
};

}
