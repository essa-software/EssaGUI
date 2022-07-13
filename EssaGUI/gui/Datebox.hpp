#pragma once

#include <EssaUtil/SimulationClock.hpp>
#include "Container.hpp"
#include "StateTextButton.hpp"
#include "TextButton.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <memory>
#include <vector>

namespace GUI
{

class Datebox : public Container
{
    Util::SimulationClock::time_point m_date;
    Textfield* m_date_textfield = nullptr;
    Textfield* m_century_textfield = nullptr;
    Textfield* m_year_textfield = nullptr;
    Textfield* m_month_textfield = nullptr;
    Container* m_calendar_container = nullptr;
    TextButton* m_toggle_container_button = nullptr;
    std::vector<std::pair<TextButton*, Util::SimulationClock::time_point>> m_calendar_contents;
    std::vector<GUI::Container*> m_calendar_rows;

    void m_create_container();
    TextButton* m_create_calendar_button(Container& c);
    void m_update_calendar();

public:
    explicit Datebox(Container& parent);

    void set_display_attributes(Util::Color bg_color, Util::Color fg_color, Util::Color text_color);
};

}
