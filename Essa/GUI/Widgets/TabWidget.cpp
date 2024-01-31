#include "TabWidget.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Text.hpp>

#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <cassert>
#include <iostream>

namespace GUI {

class TabButton : public TextButton {
private:
    virtual Theme::ButtonColors default_button_colors() const override { return theme().tab_button; }

    virtual void draw(Gfx::Painter&) const override;
};

void TabButton::draw(Gfx::Painter& window) const {
    auto colors = colors_for_state();

    Gfx::RectangleDrawOptions rect;
    rect.set_border_radius(10);
    rect.border_radius_bottom_left = 0;
    rect.border_radius_bottom_right = 0;
    rect.fill_color = colors.background;
    window.deprecated_draw_rectangle(!is_active() ? Util::Rectf { { 0, 4 }, raw_size().cast<float>() } : local_rect().cast<float>(), rect);

    Util::Point2f text_position;
    if (!is_active())
        text_position.set_y(2);

    Gfx::Text text { is_active() ? content() : active_content(), Application::the().font() };
    text.set_fill_color(colors.text);
    text.set_font_size(theme().label_font_size);
    text.align(Align::Center, { text_position, raw_size().cast<float>() });
    text.draw(window);
}

void TabSelectWidget::on_init() { set_layout<HorizontalBoxLayout>(); }

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
        m_buttons[s]->set_active(s == index, NotifyUser::No);
        // FIXME: There is a bug in event propagation apparently which causes
        //        sibling buttons' event handler to be run.
        // m_buttons[s]->set_enabled(s != index);
    }
}

void TabWidget::on_init() {
    set_layout<VerticalBoxLayout>();
    m_tab_select = add_widget<TabSelectWidget>();
    m_tab_select->set_size({ Util::Length::Auto, 30.0_px });
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

void TabWidget::setup_tab(Util::UString caption, Widget* tab) {
    tab->set_background_color(theme().tab_button.active.unhovered.background);
    tab->set_size({ { 100, Util::Length::Percent }, { 100, Util::Length::Percent } });
    tab->set_visible(m_tabs.empty());
    m_tab_select->add_button(std::move(caption), m_tabs.size());
    m_tabs.push_back(tab);
}

EML::EMLErrorOr<void> TabWidget::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));
    for (auto const& child : object.objects) {
        std::shared_ptr<Widget> widget = TRY(child.construct<Widget>(loader, window_root()));
        m_tab_container->add_created_widget(widget);
        setup_tab(TRY(TRY(child.require_property("caption")).to_string()), widget.get());
    }
    return {};
}

EML_REGISTER_CLASS(TabWidget);

}
