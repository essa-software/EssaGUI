#include "ContextMenu.hpp"

#include "Application.hpp"
#include "Container.hpp"
#include "Textfield.hpp"
#include "Widget.hpp"

#include <EssaGUI/gfx/Window.hpp>
#include <LLGL/Window/Mouse.hpp>

namespace GUI {

constexpr float MenuWidth = 200;
constexpr float MenuItemHeight = 30;

Util::Vector2f MenuWidget::wanted_size() const {
    return { MenuWidth, MenuItemHeight * m_actions.size() };
}

void MenuWidget::add_action(Util::UString label) {
    m_actions.push_back(std::move(label));
}

Util::Rectf MenuWidget::item_rect(size_t index) const {
    return { 0, index * MenuItemHeight, MenuWidth, MenuItemHeight };
}

void MenuWidget::draw(Window& window) const {
    TextDrawOptions text;
    text.text_align = Align::CenterLeft;
    text.font_size = 14;
    text.fill_color = theme().menu.text;

    size_t index = 0;
    for (auto const& action : m_actions) {
        Util::Rectf background_rect = item_rect(index);
        Util::Rectf text_align_rect = background_rect;
        text_align_rect.left += 10;
        text_align_rect.width -= 10;
        text_align_rect.top -= 2; // HACK: to fix text alignment
        if (background_rect.contains(Util::Vector2f { llgl::mouse_position() } - widget_tree_root().position() - raw_position())) {
            RectangleDrawOptions hovered_background;
            hovered_background.fill_color = theme().selection.value(*this);
            window.draw_rectangle(background_rect, hovered_background);
        }
        window.draw_text_aligned_in_rect(action, text_align_rect, Application::the().font(), text);
        index++;
    }
}

void MenuWidget::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::MouseButtonPress && event.event().mouse_button.button == llgl::MouseButton::Left) {
        int action = std::floor((event.mouse_position().y() - raw_position().y()) / MenuItemHeight);
        std::cout << action << std::endl;
        if (action < 0 || static_cast<size_t>(action) >= m_actions.size())
            return;
        if (on_action)
            on_action(action);
        event.set_handled();
    }
}

class Separator : public Widget {
private:
    virtual LengthVector initial_size() const override { return { Util::Length::Auto, 10.0_px }; }

    virtual void draw(Window& window) const override {
        RectangleDrawOptions rect;
        rect.fill_color = theme().menu.foreground;
        window.draw_rectangle({ 10, raw_size().y() / 2, raw_size().x() - 20, 1 }, rect);
    }
};

ContextMenuOverlay::ContextMenuOverlay(HostWindow& window, ContextMenu context_menu, Util::Vector2f position)
    : Overlay(window, "ContextMenu")
    , m_context_menu(context_menu)
    , m_position(position) {

    auto& container = set_main_widget<Container>();
    container.set_layout<VerticalBoxLayout>();

    if (!context_menu.title().is_empty()) {
        auto title_textfield = container.add_widget<Textfield>();
        title_textfield->set_size({ Util::Length::Auto, 30.0_px });
        title_textfield->set_content(context_menu.title());
        title_textfield->set_padding(10);
        title_textfield->set_font_size(18);
        container.add_widget<Separator>();
    }

    m_menu_widget = container.add_widget<MenuWidget>();
    for (auto const& action : context_menu.actions()) {
        m_menu_widget->add_action(std::move(action.first));
    }
    m_menu_widget->on_action = [this](size_t action_index) {
        std::cout << action_index << std::endl;
        m_context_menu.actions()[action_index].second();
        close();
    };
}

Util::Vector2f ContextMenuOverlay::size() const {
    auto options_size = m_menu_widget->wanted_size();
    if (!m_context_menu.title().is_empty()) {
        options_size.y() += 40;
    }
    return options_size;
}

void ContextMenuOverlay::handle_event(llgl::Event event) {
    WidgetTreeRoot::handle_event(event);

    GUI::Event gui_event { event };

    // FIXME: Add something like close_when_clicked_outside()
    if ((gui_event.type() == llgl::Event::Type::MouseButtonPress && !full_rect().contains(Util::Vector2f { gui_event.mouse_position() } + position()))
        || (gui_event.type() == llgl::Event::Type::KeyPress && gui_event.event().key.keycode == llgl::KeyCode::Escape)) {
        close();
    }
}

void ContextMenuOverlay::draw(Window& window) {
    RectangleDrawOptions background;
    background.fill_color = theme().menu.background;
    background.outline_color = theme().menu.foreground;
    background.outline_thickness = -1;
    window.draw_rectangle(rect(), background);

    WidgetTreeRoot::draw(window);
}

}
