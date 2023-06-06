#include "ContextMenu.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>

namespace GUI {

constexpr float MenuWidth = 200;
constexpr float MenuItemHeight = 30;

Util::Size2i MenuWidget::wanted_size() const { return { MenuWidth, MenuItemHeight * m_actions.size() }; }

void MenuWidget::add_action(Util::UString label) { m_actions.push_back(std::move(label)); }

Util::Rectf MenuWidget::item_rect(size_t index) const { return { 0, index * MenuItemHeight, MenuWidth, MenuItemHeight }; }

void MenuWidget::draw(Gfx::Painter& painter) const {
    Gfx::Text text { "", Application::the().font() };
    text.set_font_size(14);
    text.set_fill_color(theme().menu.text);

    size_t index = 0;
    for (auto const& action : m_actions) {
        Util::Rectf background_rect = item_rect(index);
        Util::Rectf text_align_rect = background_rect;
        text_align_rect.left += 10;
        text_align_rect.width -= 10;
        text_align_rect.top -= 2; // HACK: to fix text alignment
        if (background_rect.contains(llgl::mouse_position() - widget_tree_root().position().to_vector() - raw_position().to_vector())) {
            Gfx::RectangleDrawOptions hovered_background;
            hovered_background.fill_color = theme().selection.value(*this);
            painter.deprecated_draw_rectangle(background_rect, hovered_background);
        }
        text.set_string(action);
        text.align(Align::CenterLeft, text_align_rect);
        text.draw(painter);
        index++;
    }
}

Widget::EventHandlerResult MenuWidget::on_mouse_button_press(Event::MouseButtonPress const& event) {
    if (event.button() == llgl::MouseButton::Left) {
        int action = std::floor((event.local_position().y() - raw_position().y()) / MenuItemHeight);
        std::cout << action << std::endl;
        if (action < 0 || static_cast<size_t>(action) >= m_actions.size())
            return EventHandlerResult::NotAccepted;
        if (on_action)
            on_action(action);
        return EventHandlerResult::Accepted;
    }
    return EventHandlerResult::NotAccepted;
}

class Separator : public Widget {
private:
    virtual LengthVector initial_size() const override { return { Util::Length::Auto, 10.0_px }; }

    virtual void draw(Gfx::Painter& painter) const override {
        Gfx::RectangleDrawOptions rect;
        rect.fill_color = theme().menu.foreground;
        painter.deprecated_draw_rectangle({ 10, static_cast<float>(raw_size().y()) / 2, static_cast<float>(raw_size().x()) - 20, 1 }, rect);
    }
};

ContextMenuOverlay::ContextMenuOverlay(HostWindow& window, ContextMenu context_menu, Util::Point2i position)
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

Util::Size2i ContextMenuOverlay::size() const {
    auto options_size = m_menu_widget->wanted_size();
    if (!m_context_menu.title().is_empty()) {
        options_size.set_y(options_size.y() + 40);
    }
    return options_size;
}

void ContextMenuOverlay::handle_event(Event const& event) {
    WidgetTreeRoot::handle_event(event);

    // FIXME: Add something like close_when_clicked_outside()
    if (auto mousepress = event.get<Event::MouseButtonPress>();
        mousepress && full_rect().contains(mousepress->local_position() + position().to_vector())) {
        close();
    }
    if (auto keypress = event.get<Event::KeyPress>(); keypress && keypress->code() == llgl::KeyCode::Escape) {
        close();
    }
}

void ContextMenuOverlay::draw(Gfx::Painter& painter) {
    Gfx::RectangleDrawOptions background;
    background.fill_color = theme().menu.background;
    background.outline_color = theme().menu.foreground;
    background.outline_thickness = -1;
    painter.deprecated_draw_rectangle(rect().cast<float>(), background);

    Gfx::ClipViewScope scope(painter, rect(), Gfx::ClipViewScope::Mode::NewStack);
    WidgetTreeRoot::draw(painter);
}

}
