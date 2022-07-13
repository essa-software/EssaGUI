#include "Datebox.hpp"

#include "ArrowButton.hpp"
#include "Container.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

Util::UString const month_names[] = {
    "January", "February", "March",
    "April", "May", "June",
    "July", "August", "September",
    "October", "November", "December"
};

namespace GUI {

Datebox::Datebox(Container& parent)
    : Container(parent)
    , m_date(Util::SimulationTime::create(2022, 4, 24)) {
    set_layout<VerticalBoxLayout>().set_spacing(10);
    auto main_container = add_widget<Container>();
    main_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    main_container->set_size({ Length::Auto, 30.0_px });
    m_date_textfield = main_container->add_widget<Textfield>();

    // TODO: Make Dates support UString natively
    std::ostringstream str;
    str << m_date;
    m_date_textfield->set_content(Util::UString { str.str() });

    m_toggle_container_button = main_container->add_widget<TextButton>();
    m_toggle_container_button->set_size({ 50.0_px, Length::Auto });
    m_toggle_container_button->set_alignment(Align::Center);
    m_toggle_container_button->set_toggleable(true);
    m_toggle_container_button->set_content("Show");
    m_toggle_container_button->set_active_content("Hide");
    // m_date = Util::SimulationTime::create(1602, 7, 21);
    m_create_container();

    m_toggle_container_button->on_change = [this](bool state) {
        m_calendar_container->set_visible(state);
        m_update_calendar();
        if (state)
            set_size({ Length::Auto, 280.0_px });
        else
            set_size({ Length::Auto, 30.0_px });
    };

    m_calendar_container->set_visible(false);
}

TextButton* Datebox::m_create_calendar_button(Container& c) {
    auto btn = c.add_widget<TextButton>();
    btn->set_content("");
    btn->set_active_content("");
    btn->set_alignment(Align::Center);
    btn->on_click = [btn, this]() mutable {
        unsigned i = 0;
        for (auto& c : m_calendar_contents) {
            if (c.first == btn)
                m_date = m_calendar_contents[i].second;
            i++;
        }

        m_update_calendar();
    };

    return btn;
}

void Datebox::m_create_container() {
    m_calendar_container = add_widget<Container>();
    m_calendar_container->set_layout<VerticalBoxLayout>().set_spacing(2);
    m_calendar_container->set_background_color(Util::Colors::white);

    auto century_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    century_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    century_toggle_container->set_size({ Length::Auto, 20.0_px });
    auto left_century_arrow_btn = century_toggle_container->add_widget<ArrowButton>();
    left_century_arrow_btn->set_arrow_color(Util::Color { 200, 200, 200 });
    left_century_arrow_btn->set_arrow_size(10);
    left_century_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    m_century_textfield = century_toggle_container->add_widget<Textfield>();
    m_century_textfield->set_alignment(Align::Center);
    m_century_textfield->set_display_attributes(Util::Colors::white, Util::Colors::white, Util::Color { 200, 200, 200 });
    auto right_century_arrow_btn = century_toggle_container->add_widget<ArrowButton>();
    right_century_arrow_btn->set_arrow_color(Util::Color { 200, 200, 200 });
    right_century_arrow_btn->set_arrow_size(10);
    right_century_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto year_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    year_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    year_toggle_container->set_size({ Length::Auto, 20.0_px });
    auto left_year_arrow_btn = year_toggle_container->add_widget<ArrowButton>();
    left_year_arrow_btn->set_arrow_color(Util::Color { 200, 200, 200 });
    left_year_arrow_btn->set_arrow_size(10);
    left_year_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    m_year_textfield = year_toggle_container->add_widget<Textfield>();
    m_year_textfield->set_alignment(Align::Center);
    m_year_textfield->set_display_attributes(Util::Colors::white, Util::Colors::white, Util::Color { 200, 200, 200 });
    auto right_year_arrow_btn = year_toggle_container->add_widget<ArrowButton>();
    right_year_arrow_btn->set_arrow_color(Util::Color { 200, 200, 200 });
    right_year_arrow_btn->set_arrow_size(10);
    right_year_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto month_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    month_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    month_toggle_container->set_size({ Length::Auto, 20.0_px });
    auto left_month_arrow_btn = month_toggle_container->add_widget<ArrowButton>();
    left_month_arrow_btn->set_arrow_color(Util::Color { 200, 200, 200 });
    left_month_arrow_btn->set_arrow_size(10);
    left_month_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    m_month_textfield = month_toggle_container->add_widget<Textfield>();
    m_month_textfield->set_alignment(Align::Center);
    m_month_textfield->set_display_attributes(Util::Colors::white, Util::Colors::white, Util::Color { 200, 200, 200 });
    auto right_month_arrow_btn = month_toggle_container->add_widget<ArrowButton>();
    right_month_arrow_btn->set_arrow_color(Util::Color { 200, 200, 200 });
    right_month_arrow_btn->set_arrow_size(10);
    right_month_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto day_selection_container = m_calendar_container->add_widget<Container>();
    day_selection_container->set_layout<VerticalBoxLayout>().set_spacing(2);
    for (unsigned i = 0; i < 6; i++) {
        auto row = day_selection_container->add_widget<Container>();
        row->set_layout<HorizontalBoxLayout>().set_spacing(2);
        row->set_size({ Length::Auto, { 100.0 / 6, Length::Percent } });
        m_calendar_rows.push_back(row);
        for (unsigned i = 0; i < 7; i++)
            m_calendar_contents.push_back({ m_create_calendar_button(*row), {} });
    }

    auto daytime_container = m_calendar_container->add_widget<Container>();
    daytime_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(5);
    daytime_container->set_size({ Length::Auto, { 100.0 / 6, Length::Percent } });
    auto hours = daytime_container->add_widget<Textbox>();
    hours->set_limit(2);
    hours->set_min_max_values(0, 23);
    hours->set_content("0");
    auto first_colon = daytime_container->add_widget<Textfield>();
    first_colon->set_alignment(Align::Top);
    first_colon->set_content(":");
    first_colon->set_font_size(30);
    first_colon->set_display_attributes(Util::Colors::white, Util::Colors::white, Util::Colors::black);
    auto minutes = daytime_container->add_widget<Textbox>();
    minutes->set_limit(2);
    minutes->set_min_max_values(0, 59);
    minutes->set_content("0");
    auto second_colon = daytime_container->add_widget<Textfield>();
    second_colon->set_alignment(Align::Top);
    second_colon->set_content(":");
    second_colon->set_font_size(30);
    second_colon->set_display_attributes(Util::Colors::white, Util::Colors::white, Util::Colors::black);
    auto seconds = daytime_container->add_widget<Textbox>();
    seconds->set_limit(2);
    seconds->set_min_max_values(0, 59);
    seconds->set_content("0");

    // std::cout << hours << " " << minutes << " " << seconds << "\n";

    left_century_arrow_btn->on_click = [this]() {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year - 100, local_tm.tm_mon + 1, local_tm.tm_mday);
        m_update_calendar();
    };

    left_year_arrow_btn->on_click = [this]() {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year - 1, local_tm.tm_mon + 1, local_tm.tm_mday);
        m_update_calendar();
    };

    left_month_arrow_btn->on_click = [this]() {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon, local_tm.tm_mday);
        m_update_calendar();
    };

    right_century_arrow_btn->on_click = [this]() {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year + 100, local_tm.tm_mon + 1, local_tm.tm_mday);
        m_update_calendar();
    };

    right_year_arrow_btn->on_click = [this]() {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year + 1, local_tm.tm_mon + 1, local_tm.tm_mday);
        m_update_calendar();
    };

    right_month_arrow_btn->on_click = [this]() {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon + 2, local_tm.tm_mday);
        m_update_calendar();
    };

    hours->on_change = [&](Util::UString const& str) {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        // TODO: Implement std::stoi as UString function
        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon + 1, local_tm.tm_mday, std::stoi(str.encode()), local_tm.tm_min, local_tm.tm_sec);
        m_update_calendar();
    };

    minutes->on_change = [&](Util::UString const& str) {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        // TODO: Implement std::stoi as UString function
        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon + 1, local_tm.tm_mday, local_tm.tm_hour, std::stoi(str.encode()), local_tm.tm_sec);
        m_update_calendar();
    };

    seconds->on_change = [&](Util::UString const& str) {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon + 1, local_tm.tm_mday, local_tm.tm_hour, local_tm.tm_min, std::stoi(str.encode()));
        m_update_calendar();
    };
}

void Datebox::m_update_calendar() {
    time_t clock = m_date.time_since_epoch().count();
    tm local_tm = *localtime(&clock);
    local_tm.tm_mday = 1;
    mktime(&local_tm);

    if (local_tm.tm_year < 0)
        m_century_textfield->set_content(Util::UString { std::to_string(1900 + (int)((local_tm.tm_year / 100 - 1) * 100)) + " - " + std::to_string(1900 + (int)((local_tm.tm_year / 100 - 1) * 100) + 99) });
    else
        m_century_textfield->set_content(Util::UString { std::to_string(1900 + (int)(local_tm.tm_year / 100 * 100)) + " - " + std::to_string(1900 + (int)(local_tm.tm_year / 100 * 100) + 99) });
    m_year_textfield->set_content(Util::UString { std::to_string(1900 + local_tm.tm_year) });
    m_month_textfield->set_content(month_names[local_tm.tm_mon]);

    int current_month_day = -(local_tm.tm_wday == 0 ? 6 : local_tm.tm_wday - 1);
    std::ostringstream str;
    str << m_date;
    m_date_textfield->set_content(Util::UString { str.str() });

    for (unsigned j = 0; j < m_calendar_contents.size(); j++) {
        current_month_day++;
        Util::SimulationClock::time_point date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon + 1, current_month_day);
        auto t = date.time_since_epoch().count();
        tm temp_tm = *localtime(&t);
        m_calendar_contents[j].second = date;

        if (j == 28 && temp_tm.tm_mday <= 14) {
            m_calendar_rows[4]->set_visible(false);
            m_calendar_rows[5]->set_visible(false);

            return;
        }
        else {
            m_calendar_rows[4]->set_visible(true);
        }

        if (j == 35 && temp_tm.tm_mday <= 7) {
            m_calendar_rows[5]->set_visible(false);

            return;
        }
        else {
            m_calendar_rows[5]->set_visible(true);
        }

        m_calendar_contents[j].first->set_content(Util::UString { std::to_string(temp_tm.tm_mday) });
        m_calendar_contents[j].first->set_active_content(Util::UString { std::to_string(temp_tm.tm_mday) });

        // TODO: Add that to theme
        auto& colors = m_calendar_contents[j].first->override_button_colors();
        if (m_date == date) {
            colors.untoggleable.background = Util::Colors::blue;
            colors.untoggleable.text = Util::Colors::white;
        }
        else {
            colors.untoggleable.background = temp_tm.tm_mon == local_tm.tm_mon ? Util::Color { 150, 150, 150 } : Util::Color { 200, 200, 200 };
            colors.untoggleable.text = temp_tm.tm_mon == local_tm.tm_mon ? Util::Colors::black : Util::Color { 50, 50, 50 };
        }
    }
}

}
