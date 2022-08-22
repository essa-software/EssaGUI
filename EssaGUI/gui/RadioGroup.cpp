#include "EssaGUI/gui/RadioGroup.hpp"
#include "RadioButton.hpp"

namespace GUI {
RadioGroup::RadioGroup(Container& c)
    : Container(c) {
    set_layout<VerticalBoxLayout>().set_spacing(5);
}

void RadioGroup::do_update() {
    size_t i = 0;
    size_t temp = m_index;

    for (const auto& radio : m_buttons) {
        if (radio->is_active() && i != m_index) {
            temp = i;
            radio->set_active(true);
        }else{
            radio->set_active(false);
        }

        radio->set_size({Length::Auto, m_row_height});
        i++;
    }

    if(temp != m_index){
        m_index = temp;
        
        if(on_change)
            on_change(m_index);
    }


    if(m_index < m_buttons.size())
        m_buttons[m_index]->set_active(true);
}

void RadioGroup::set_index(size_t index) {
    m_index = index;

    size_t i = 0;

    if(on_change)
        on_change(m_index);

    for (const auto& radio : m_buttons) {
        if (i == m_index) 
            radio->set_active(true);
        else
            radio->set_active(false);
        i++;
    }
}

void RadioGroup::add_radio(const Util::UString& caption) {
    m_buttons.push_back(add_widget<RadioButton>());
    m_buttons.back()->set_size({Length::Auto, m_row_height});
    m_buttons.back()->set_caption(caption);
}

}
