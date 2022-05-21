#pragma once

#include "ArrowButton.hpp"
#include "Textfield.hpp"
#include "Widget.hpp"
#include <memory>
#include <string>
#include <vector>

namespace GUI {

template<typename T>
class SelectWidget : public Widget {
    std::shared_ptr<Container> m_container;
    std::shared_ptr<Container> m_elements_container;

    Textfield* m_label;
    Textfield* m_textfield;

    std::shared_ptr<ArrowButton> m_expand_button;
    std::map<std::string, T> m_contents;
    unsigned m_index;
    bool expanded = false;

public:
    explicit SelectWidget(Container& c);

    void add_entry(std::string label, T value);
    void set_label(std::string label) { m_label->set_content(label); }
};

}
