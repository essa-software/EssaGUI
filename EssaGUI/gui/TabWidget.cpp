#include "TabWidget.hpp"

#include "Application.hpp"
#include "Container.hpp"

#include <EssaGUI/gfx/Window.hpp>
#include <cassert>
#include <iostream>

namespace GUI {

class TabButton : public TextButton {
private:
    virtual Theme::ButtonColors default_button_colors() const override { return theme().tab_button; }

    virtual void draw(GUI::Window&) const override;
};

void TabButton::draw(GUI::Window& window) const {
    auto colors = colors_for_state();

    RectangleDrawOptions rect;
    rect.set_border_radius(10);
    rect.border_radius_bottom_left = 0;
    rect.border_radius_bottom_right = 0;
    rect.fill_color = colors.background;
    window.draw_rectangle(!is_active() ? Util::Rectf { { 0, 4 }, size() } : local_rect(), rect);

    Util::Vector2f text_position;
    if (!is_active())
        text_position.y() = 2;
    TextDrawOptions text;
    text.fill_color = colors.text;
    text.font_size = theme().label_font_size;
    text.text_align = Align::Center;
    window.draw_text_aligned_in_rect(is_active() ? content() : active_content(), { text_position, size() }, Application::the().font(), text);
}

void TabSelectWidget::on_init() {
    set_layout<HorizontalBoxLayout>();
}

void TabSelectWidget::add_button(Util::UString caption, size_t tab_index) {
    auto button = add_widget<TabButton>();
    button->set_active_content(caption);
    button->set_content(std::move(caption));
    button->set_active(tab_index == 0);
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

void TabWidget::on_init() {
    set_layout<VerticalBoxLayout>();
    m_tab_select = add_widget<TabSelectWidget>();
    m_tab_select->set_size({ Length::Auto, 30.0_px });
    m_tab_container = add_widget<Container>();
    m_tab_container->set_layout<BasicLayout>();
}

void TabWidget::switch_to_tab(size_t index) {
    if (m_index == index)
        return;

    assert(index < m_tabs.size());

    if (on_tab_switch)
        on_tab_switch(index);
    m_index = index;

    for (size_t s = 0; s < m_tabs.size(); s++)
        m_tabs[s]->set_visible(s == index);
    m_tab_select->switch_to_tab(index);
}

void TabWidget::setup_tab(Util::UString caption, Container* tab) {
    tab->set_background_color(theme().tab_button.active.unhovered.background);
    tab->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
    tab->set_visible(m_tabs.size() == 0);
    m_tab_select->add_button(std::move(caption), m_tabs.size());
    m_tabs.push_back(tab);
}

}
