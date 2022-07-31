#pragma once
#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/TextButton.hpp"
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <cstddef>
#include <functional>
#include <vector>

namespace GUI {

class Listbox : public Container {
public:
    explicit Listbox(Container& c)
        : Container(c) {
        set_layout<VerticalBoxLayout>();
    }

    virtual void do_update();

    void add(Util::UString const& label);
    void remove(Util::UString const& label);

    void sorted_list(bool con) {
        m_sort_list = con;
        sort_vector();
    }

    bool sorted_list() const { return m_sort_list; }

    void allow_multichoose(bool con) { m_allow_multichoose = con; }
    bool allow_multichoose() const { return m_allow_multichoose; }

    void set_row_height(Length const& height);
    Length get_row_height() const { return m_row_height; }

    std::function<void(size_t, bool)> on_change;

    std::vector<size_t> indexes() const { return m_allow_multichoose ? m_index_vector : std::vector<size_t>(1, m_index); }
    void set_index(size_t index) {
        m_index = index;
        m_index_vector.clear();
        m_index_vector.push_back(index);
    }

    void sort_vector();

private:
    bool m_sort_list = false;
    bool m_allow_multichoose = false;
    Length m_row_height = 15.0_px;

    size_t m_index;
    std::vector<size_t> m_index_vector;

    std::vector<TextButton*> m_list_buttons;
};

}
