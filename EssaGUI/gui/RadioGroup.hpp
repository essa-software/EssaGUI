#pragma once

#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/RadioButton.hpp"
#include <EssaUtil/UString.hpp>
#include <functional>
#include <vector>

namespace GUI {

class RadioGroup : public Container {
public:
    virtual void on_add() override;

    void add_radio(Util::UString const& caption);
    void set_index(size_t index);
    size_t get_index() const { return m_index; }
    virtual void do_update() override;
    void set_row_height(Length height) { m_row_height = height; }
    Length get_row_height() const { return m_row_height; }

    std::function<void(size_t)> on_change;

private:
    size_t m_index = 0;
    Length m_row_height = 15.0_px;

    std::vector<RadioButton*> m_buttons;
};

}
