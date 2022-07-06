#include "SelectWidget.hpp"

#include <EssaUtil/Units.hpp>
#include "Container.hpp"
#include "Textfield.hpp"

namespace GUI {

template<typename T>
SelectWidget<T>::SelectWidget(Container& c)
    : Widget(c) {
    auto& layout = m_container->set_layout<HorizontalBoxLayout>();
    layout.set_spacing(0);
    m_label = m_container->add_widget<Textfield>();
    m_textfield = m_container->add_widget<Textfield>();
    m_textfield->set_display_attributes(sf::Color(220, 220, 220), sf::Color(120, 120, 120), sf::Color(30, 30, 30));

    m_elements_container->set_layout<VerticalBoxLayout>();
    m_elements_container->set_size({ Length::Auto, 0.0_px });

    m_expand_button->on_change = [this]() {
        this->m_elements_container->set_size({ Length::Auto, this->m_contents.size() });
    };
}

}
