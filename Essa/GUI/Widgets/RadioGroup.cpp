#include "RadioGroup.hpp"

#include "RadioButton.hpp"

namespace GUI {

void RadioGroup::set_index(size_t index) {
    assert(index < m_radio_buttons.size());
    m_radio_buttons[index]->set_active(true);
}

void RadioGroup::relayout() {
    Container::relayout();

    m_radio_buttons.clear();

    visit_children([&](auto& widget) {
        if (Util::is<RadioButton>(widget)) {
            add_radio_button(static_cast<RadioButton&>(widget));
        }
    });
}

void RadioGroup::add_radio_button(RadioButton& button) {
    m_radio_buttons.push_back(&button);
    button.on_change = [&, index = m_radio_buttons.size() - 1](bool state) {
        if (state) {
            m_index = index;
        }
        for (auto* other : m_radio_buttons) {
            other->set_active(other == &button, NotifyUser::No);
        }
        if (on_change) {
            on_change(index);
        }
    };
}

EML_REGISTER_CLASS(RadioGroup);

}
