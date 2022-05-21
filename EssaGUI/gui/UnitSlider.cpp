#include "UnitSlider.hpp"

#include "NotifyUser.hpp"
#include "StateTextButton.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace GUI {

static std::string serialize_value(double value, double step) {
    std::ostringstream oss;
    oss << std::fixed;
    if (step < 1)
        oss << std::setprecision(std::ceil(-std::log10(step)));
    else
        oss << std::setprecision(0);
    oss << value;
    return oss.str();
}

UnitSlider::UnitSlider(Container& parent, Util::Quantity q, double min, double max, double step)
    : Container(parent) {

    auto& layout = set_layout<HorizontalBoxLayout>();
    layout.set_spacing(5);
    m_name_textfield = add_widget<Textfield>();
    m_name_textfield->set_size({ 100.0_px, Length::Auto });
    m_slider = add_widget<Slider>(min, max, step);
    m_slider->on_change = [this](double value) {
        m_textbox->set_content(serialize_value(value, m_slider->step()), NotifyUser::No);
        if (on_change)
            on_change(value);
    };
    m_textbox = add_widget<Textbox>();
    m_textbox->set_data_type(Textbox::NUMBER);
    m_textbox->set_content(serialize_value(m_slider->get_value(), m_slider->step()));
    m_textbox->on_change = [this](std::string const& value) {
        if (value.empty())
            m_slider->set_value(0, NotifyUser::No);
        try {
            // Notify user so that they get on_change().
            auto value_as_number = std::stod(value);
            m_slider->set_value(value_as_number, NotifyUser::No);
            if (on_change)
                on_change(value_as_number);
        } catch (...) {
            ;
        }
    };
    m_unit_button = add_widget<StateTextButton<unsigned>>();
    m_unit_button->set_size({ 100.0_px, Length::Auto });
    m_unit_button->set_alignment(Align::Center);

    unsigned i = 0;

    for(const auto& u : Util::s_units.find(m_unit)->second){
        m_unit_button->add_state(u.string, i, sf::Color::Green);
        std::cout << u.string << " " << i << "\n";
        i++;
    }

    m_unit_button->on_change = [this](unsigned index){
        auto unit = Util::s_units.find(m_unit)->second;

        unsigned prev_index;
        if(index == 0)
            prev_index = unit.size() - 1;
        else
            prev_index = index - 1;
        
        double mul = 1.0 / unit[prev_index].multiplier * unit[index].multiplier;
        
        m_slider->set_value(m_slider->get_value() * mul);
        m_slider->set_range(m_slider->min_value() * mul, m_slider->max_value() * mul, m_slider->step() * mul);
    };
}

double UnitSlider::value() const {
    return m_slider->get_value();
}

void UnitSlider::set_value(double value) {
    m_slider->set_value(value);
    m_textbox->set_content(std::to_string(value));
}

}
