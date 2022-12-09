#pragma once

#include <Essa/GUI/Widgets/Container.hpp>
#include "StateTextButton.hpp"
#include "TextButton.hpp"
#include "Textfield.hpp"
#include <EssaUtil/SimulationClock.hpp>
#include <memory>
#include <vector>

namespace GUI {

class Datebox : public Container {
public:
    virtual void on_init() override;

private:
    Util::SimulationClock::time_point m_date;
    Textfield* m_date_textfield = nullptr;
    Textfield* m_century_textfield = nullptr;
    Textfield* m_year_textfield = nullptr;
    Textfield* m_month_textfield = nullptr;
    Container* m_calendar_container = nullptr;
    TextButton* m_toggle_container_button = nullptr;
    std::vector<std::pair<TextButton*, Util::SimulationClock::time_point>> m_calendar_contents;
    std::vector<GUI::Container*> m_calendar_rows;

    void create_container();
    TextButton* create_calendar_button(Container& c);
    void update_calendar();

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
