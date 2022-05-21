#include "ColorPicker.hpp"

#include "Container.hpp"
#include "Textfield.hpp"

namespace GUI {

void ColorPickerDisplay::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rs({ size().x - 10, size().y - 10 });
    rs.setPosition(5, 5);
    rs.setFillColor(m_color);
    window.draw(rs);
}

ColorPicker::ColorPicker(Container& c)
    : Container(c) {
    auto& layout = set_layout<HorizontalBoxLayout>();
    layout.set_spacing(10);

    auto sliders_container = add_widget<Container>();
    m_color_picker_display = add_widget<ColorPickerDisplay>();
    m_color_picker_display->set_size({ 100.0_px, Length::Auto });
    m_color_picker_display->set_color(sf::Color(127, 127, 127));

    auto& sliders_container_layout = sliders_container->set_layout<VerticalBoxLayout>();
    sliders_container_layout.set_spacing(10);
    {
        auto create_color_slider = [&](std::string component) {
            auto container = sliders_container->add_widget<Container>();
            auto& layout = container->set_layout<HorizontalBoxLayout>();
            layout.set_spacing(5);
            auto label = container->add_widget<Textfield>();
            label->set_content(std::move(component));
            label->set_size({ 20.0_px, Length::Auto });
            auto slider = container->add_widget<Slider>(0, 255);
            auto value_textfield = container->add_widget<Textfield>();
            value_textfield->set_size({ 40.0_px, Length::Auto });
            slider->on_change = [this, value_textfield](double slider_value) {
                value_textfield->set_content(std::to_string((int)slider_value));
                m_color_picker_display->set_color(value());
                if (on_change)
                    on_change(value());
            };
            value_textfield->set_content("127");
            return slider;
        };

        m_r_slider = create_color_slider("R");
        m_g_slider = create_color_slider("G");
        m_b_slider = create_color_slider("B");
    }
}

sf::Color ColorPicker::value() const {
    return sf::Color {
        static_cast<uint8_t>(m_r_slider->get_value()),
        static_cast<uint8_t>(m_g_slider->get_value()),
        static_cast<uint8_t>(m_b_slider->get_value())
    };
}

void ColorPicker::set_value(sf::Color color){
    m_r_slider->set_value(color.r);
    m_g_slider->set_value(color.g);
    m_b_slider->set_value(color.b);
}

}
