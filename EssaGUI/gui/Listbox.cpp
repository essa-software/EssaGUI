#include "Listbox.hpp"

#include <EssaGUI/gui/NotifyUser.hpp>
#include <EssaGUI/gui/TextButton.hpp>
#include <EssaUtil/UString.hpp>
#include <algorithm>
#include <cstddef>
#include <vector>

namespace GUI {

void Listbox::sort_vector() {
    if (m_sort_list) {
        std::vector<Util::UString> vec;

        for (const auto& btn : m_list_buttons) {
            vec.push_back(btn->content());
        }

        std::stable_sort(vec.begin(), vec.end(), [](Util::UString const& lhs, Util::UString const& rhs) {
            return lhs < rhs;
        });

        for (size_t i = 0; i < m_list_buttons.size(); i++) {
            m_list_buttons[i]->set_content(vec[i]);
        }
    }
}

void Listbox::add(const Util::UString& label) {
    auto btn = add_widget<TextButton>();
    btn->set_content(label);
    btn->set_active_content(label);
    btn->set_alignment(Align::Center);
    btn->set_size({ Length::Auto, m_row_height });
    btn->set_toggleable(true);

    if (!m_allow_multichoose && m_index_vector.size() == 0) {
        btn->set_active(true);
        m_index_vector.push_back(0);
    }

    m_list_buttons.push_back(btn);

    btn->on_change = [this, i = m_list_buttons.size() - 1](bool state) {
        if (m_allow_multichoose) {
            size_t index = 0;
            m_index_vector.clear();

            for (const auto& btn : m_list_buttons) {
                if (btn->is_active()) {
                    m_index_vector.push_back(index);
                }
                index++;
            }
        }
        else {
            if (i == m_index_vector.front()) {
                return;
            }

            m_list_buttons[m_index_vector.front()]->set_active(false, NotifyUser::No);
            m_list_buttons[i]->set_active(true, NotifyUser::No);
            m_index_vector.front() = i;
        }

        if (on_change)
            on_change(i, state);
    };

    sort_vector();
}

void Listbox::remove(const Util::UString& label) {
    std::vector<TextButton*> new_vec;
    std::vector<size_t> new_indexes;

    for (size_t i = 0; i < m_list_buttons.size(); i++) {
        if (m_list_buttons[i]->content() != label) {
            new_vec.push_back(m_list_buttons[i]);

            if (m_list_buttons[i]->is_active()) {
                new_indexes.push_back(i);
            }
        }
    }

    m_list_buttons = std::move(new_vec);
    m_index_vector = std::move(new_indexes);
}

void Listbox::set_row_height(const Length& height) {
    m_row_height = height;

    for (const auto& btn : m_list_buttons) {
        btn->set_size({ Length::Auto, m_row_height });
    }
}

}
