#include "SelectWidget.hpp"

#include "Textfield.hpp"
#include <Essa/GUI/Widgets/Container.hpp>
#include <EssaUtil/Units.hpp>

namespace GUI {

template<typename T>
void SelectWidget<T>::on_init() {
    auto& layout = m_container->set_layout<HorizontalBoxLayout>();
    layout.set_spacing(0);
    m_label = m_container->add_widget<Textfield>();
    m_textfield = m_container->add_widget<Textfield>();
    // m_textfield->set_display_attributes(Util::Color(220, 220, 220), Util::Color(120, 120, 120), Util::Color(30, 30, 30));

    m_elements_container->set_layout<VerticalBoxLayout>();
    m_elements_container->set_size({ Util::Length::Auto, 0.0_px });

    m_expand_button->on_change = [this]() {
        this->m_elements_container->set_size({ Util::Length::Auto, this->m_contents.size() });
    };
}

}
