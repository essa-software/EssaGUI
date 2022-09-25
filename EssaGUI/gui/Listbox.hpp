#pragma once

#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/TextButton.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <cstddef>
#include <functional>
#include <vector>

namespace GUI {

class Listbox : public Container {
public:
    virtual void on_init() override {
        set_layout<VerticalBoxLayout>();
    }

    void add(Util::UString const& label);
    void remove(Util::UString const& label);

    void sorted_list(bool con) {
        m_sort_list = con;
        sort_vector();
    }

    bool sorted_list() const { return m_sort_list; }

    void allow_multichoose(bool con) {
        m_allow_multichoose = con;
        if (!m_allow_multichoose) {
            for (size_t i = 1; i < m_index_vector.size(); i++) {
                m_list_buttons[i]->set_active(false, NotifyUser::No);
            }

            m_list_buttons.resize(1);
        }
    }
    bool allow_multichoose() const { return m_allow_multichoose; }

    void set_row_height(Util::Length const& height);
    Util::Length get_row_height() const { return m_row_height; }

    std::function<void(size_t, bool)> on_change;

    std::vector<size_t> indexes() const { return m_index_vector; }
    void set_index(size_t index) {
        m_index_vector.clear();
        m_index_vector.push_back(index);
    }

    void sort_vector();

private:
    bool m_sort_list = false;
    bool m_allow_multichoose = false;
    Util::Length m_row_height = 15.0_px;

    std::vector<size_t> m_index_vector;

    std::vector<TextButton*> m_list_buttons;
};

}
