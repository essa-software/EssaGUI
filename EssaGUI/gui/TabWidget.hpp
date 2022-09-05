#pragma once

#include "Container.hpp"
#include "TextButton.hpp"
#include <functional>

namespace GUI {

class TabWidget;

class TabSelectWidget : public Container {
public:
    virtual void on_add() override;

    void add_button(Util::UString caption, size_t tab_index);
    void switch_to_tab(size_t index);

private:
    std::vector<TextButton*> m_buttons;
};

class TabWidget : public Container {
public:
    virtual void on_add() override;

    template<class... Args>
    Container& add_tab(Util::UString caption, Args&&... args) {
        auto tab = m_tab_container->add_widget<Container>(std::forward<Args>(args)...);
        setup_tab(caption, tab);
        return *tab;
    }

    void switch_to_tab(size_t index);
    unsigned index() const { return m_index; }

    std::function<void(unsigned)> on_tab_switch;

private:
    unsigned m_index = 0;
    void setup_tab(Util::UString caption, Container* tab);

    TabSelectWidget* m_tab_select {};
    Container* m_tab_container {};
    std::vector<Container*> m_tabs;
};

}
