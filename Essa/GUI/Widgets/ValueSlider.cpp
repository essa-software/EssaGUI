#include "ValueSlider.hpp"

#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace GUI {

static Util::UString serialize_value(double value, double step) {
    std::ostringstream oss;
    oss << std::fixed;
    if (step < 1)
        oss << std::setprecision(std::ceil(-std::log10(step)));
    else
        oss << std::setprecision(0);
    oss << value;
    return Util::UString { oss.str() };
}

void ValueSlider::on_init() {
    auto& layout = set_layout<HorizontalBoxLayout>();
    layout.set_spacing(5);
    m_name_textfield = add_widget<Textfield>();
    m_name_textfield->set_size({ 100.0_px, Util::Length::Auto });
    m_slider = add_widget<Slider>();
    m_slider->on_change = [this](double value) {
        m_textbox->set_content(serialize_value(value, m_slider->step()), NotifyUser::No);
        if (on_change)
            on_change(value);
    };
    m_textbox = add_widget<Textbox>();
    m_textbox->set_type(Textbox::NUMBER);
    m_textbox->set_content(serialize_value(m_slider->value(), m_slider->step()));
    m_textbox->on_change = [this](Util::UString const& value) {
        if (value.is_empty())
            m_slider->set_value(0, NotifyUser::No);
        // Notify user so that they get on_change().
        if (auto maybe_value_as_number = value.parse<double>(); !maybe_value_as_number.is_error()) {
            auto value_as_number = maybe_value_as_number.release_value();
            m_slider->set_value(value_as_number, NotifyUser::No);
            if (on_change)
                on_change(value_as_number);
        }
    };
    m_unit_textfield = add_widget<Textfield>();
    m_unit_textfield->set_size({ 50.0_px, Util::Length::Auto });
}

double ValueSlider::value() const {
    return m_slider->value();
}

void ValueSlider::set_value(double value, NotifyUser notify_user) {
    m_slider->set_value(value, notify_user);
    m_textbox->set_content(Util::UString { std::to_string(m_slider->value()) }, notify_user);
}

EML::EMLErrorOr<void> ValueSlider::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    // Deliberately skipping Container so that you can't override things like layout here
    TRY(Widget::load_from_eml_object(object, loader)); // NOLINT(bugprone-parent-virtual-call)

    set_min(TRY(object.get_property("min", EML::Value(static_cast<double>(min()))).to_double()));
    set_max(TRY(object.get_property("max", EML::Value(static_cast<double>(max()))).to_double()));
    set_step(TRY(object.get_property("step", EML::Value(static_cast<double>(step()))).to_double()));
    set_value(TRY(object.get_property("value", EML::Value(static_cast<double>(value()))).to_double()));
    set_name(TRY(object.get_property("name", EML::Value("")).to_string()));
    set_unit(TRY(object.get_property("unit", EML::Value("")).to_string()));
    set_name_textfield_size(TRY(object.get_property("name_textfield_size", EML::Value(100.0_px)).to_length()));
    set_unit_textfield_size(TRY(object.get_property("unit_textfield_size", EML::Value(50.0_px)).to_length()));
    return {};
}

EML_REGISTER_CLASS(ValueSlider);

}
