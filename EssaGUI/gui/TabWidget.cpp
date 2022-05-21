#include "TabWidget.hpp"

#include <EssaGUI/gfx/RoundedEdgeRectangleShape.hpp>
#include "Application.hpp"
#include "Container.hpp"

#include <cassert>
#include <iostream>

namespace GUI {

class TabButton : public TextButton {
public:
    TabButton(Container& c)
        : TextButton(c) { }

    static sf::Color const BgColor;

private:
    virtual void draw(sf::RenderWindow&) const override;
};

sf::Color const TabButton::BgColor { 120, 120, 120, 100 };

void TabButton::draw(sf::RenderWindow& window) const {
    RoundedEdgeRectangleShape rect(size(), 10);
    rect.set_border_radius_bottom_left(0);
    rect.set_border_radius_bottom_right(0);
    rect.setFillColor(bg_color_for_state());
    if (!is_active())
        rect.move(0, 4);
    window.draw(rect);

    sf::Text text(content(), Application::the().font, 15);
    text.setFillColor(text_color_for_state());
    align_text(get_alignment(), size(), text);

    if (is_active())
        text.setString(active_content());
    else
        text.move(0, 2);
    window.draw(text);
}

TabSelectWidget::TabSelectWidget(Container& c)
    : Container(c) {
    set_layout<HorizontalBoxLayout>();
}

void TabSelectWidget::add_button(std::string caption, size_t tab_index) {
    auto button = add_widget<TabButton>();
    button->set_active_content(caption);
    button->set_content(std::move(caption));
    button->set_active(tab_index == 0);
    button->set_active_display_attributes(TabButton::BgColor + sf::Color(60, 60, 60, 0), sf::Color::Transparent, sf::Color::White);
    button->set_display_attributes(sf::Color(80, 80, 80, 100),
        sf::Color::Transparent,
        sf::Color(180, 180, 180));
    button->on_click = [this, tab_index]() {
        switch_to_tab(tab_index);
        static_cast<TabWidget*>(parent())->switch_to_tab(tab_index);
    };
    button->set_toggleable(true);
    button->set_alignment(GUI::Align::Center);
    m_buttons.push_back(button);
}

void TabSelectWidget::switch_to_tab(size_t index) {
    for (size_t s = 0; s < m_buttons.size(); s++) {
        m_buttons[s]->set_active_without_action(s == index);
        // FIXME: There is a bug in event propagation apparently which causes
        //        sibling buttons' event handler to be run.
        // m_buttons[s]->set_enabled(s != index);
    }
}

TabWidget::TabWidget(Container& c)
    : Container(c) {
    set_layout<VerticalBoxLayout>();
    m_tab_select = add_widget<TabSelectWidget>();
    m_tab_select->set_size({ Length::Auto, 30.0_px });
    m_tab_container = add_widget<Container>();
    m_tab_container->set_layout<BasicLayout>();
}

void TabWidget::switch_to_tab(size_t index) {
    assert(index < m_tabs.size());

    if(on_tab_switch)
        on_tab_switch(index);
    m_index = index;
    
    for (size_t s = 0; s < m_tabs.size(); s++)
        m_tabs[s]->set_visible(s == index);
    m_tab_select->switch_to_tab(index);
}

void TabWidget::setup_tab(std::string caption, Container* tab) {
    tab->set_background_color(TabButton::BgColor);
    tab->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
    tab->set_visible(m_tabs.size() == 0);
    m_tab_select->add_button(std::move(caption), m_tabs.size());
    m_tabs.push_back(tab);
}

}
