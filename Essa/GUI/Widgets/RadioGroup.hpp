#pragma once

#include "EssaUtil/Config.hpp"
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/RadioButton.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Is.hpp>
#include <EssaUtil/UString.hpp>
#include <functional>
#include <vector>

namespace GUI {

// Container that handles mutual exclusivity of RadioButtons.
class RadioGroup : public Container {
public:
    RadioButton* add_radio(Util::UString const& label) {
        auto* radio_button = add_widget<RadioButton>();
        radio_button->set_caption(label);
        return radio_button;
    }

    void set_index(size_t index);
    size_t get_index() const { return m_index; }

    std::function<void(size_t)> on_change;

private:
    virtual void relayout() override;

    void add_radio_button(RadioButton&);

    size_t m_index = 0;
    std::vector<RadioButton*> m_radio_buttons;
};

}
