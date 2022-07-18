#include "ContextMenu.hpp"
#include "EssaGUI/gfx/Window.hpp"

#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Widget.hpp>
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
    RectangleDrawOptions background;
    background.fill_color = theme().menu.background;
    background.outline_color = theme().menu.foreground;
    background.outline_thickness = -1;
    window.draw_rectangle(local_rect(), background);

    TextDrawOptions text;
    text.text_align = Align::CenterLeft;
    text.font_size = 15;
    text.fill_color = theme().menu.text;

    size_t index = 0;
    for (auto const& action : m_actions) {
        Util::Rectf background_rect = item_rect(index);
        Util::Rectf text_align_rect = background_rect;
        text_align_rect.left += 10;
        text_align_rect.width -= 10;
        if (background_rect.contains(Util::Vector2f { llgl::mouse_position() } - widget_tree_root().position())) {
            RectangleDrawOptions hovered_background;
            hovered_background.fill_color = theme().selection;
            window.draw_rectangle(background_rect, hovered_background);
        }
        window.draw_text_aligned_in_rect(action, text_align_rect, Application::the().font, text);
        index++;
    }
}

void MenuWidget::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::MouseButtonPress && event.event().mouse_button.button == llgl::MouseButton::Left) {
        int action = event.mouse_position().y() / MenuItemHeight;
        if (action < 0 || static_cast<size_t>(action) >= m_actions.size())
            return;
        if (on_action)
            on_action(m_actions[action]);
        event.set_handled();
    }
}

ContextMenuOverlay::ContextMenuOverlay(GUI::Window& wnd, ContextMenu context_menu)
    : Overlay(wnd, "ContextMenu")
    , m_position(context_menu.position) {
    m_menu_widget = &set_main_widget<MenuWidget>();
    for (auto const& action : context_menu.actions) {
        add_action(action.first, std::move(action.second));
        m_menu_widget->add_action(std::move(action.first));
    }
    m_menu_widget->on_action = [this](Util::UString const& action) {
        m_actions[action]();
        close();
    };
}

void ContextMenuOverlay::add_action(Util::UString label, std::function<void()> callback) {
    m_actions.insert({ label, std::move(callback) });
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

void ContextMenuOverlay::handle_events() {
    // This event handler just takes all the events
    // (except global events) and passes the to the
    // underlying main_widget. This is used for modal
    // windows.
    // FIXME: Support moving other ToolWindows even
    //        if other modal window is open.
    // FIXME: This is copied from ToolWindow. Add
    //        some way to deduplicate this.
    llgl::Event event;
    while (window().poll_event(event)) {
        handle_event(transform_event(position(), event));
        if (event.type == llgl::Event::Type::Resize)
            Application::the().handle_event(event);
    }
}

}
