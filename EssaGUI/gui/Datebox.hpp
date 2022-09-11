#pragma once

#include "Container.hpp"
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
};

}
