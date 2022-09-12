#include "Application.hpp"

#include "ContextMenu.hpp"
#include "EssaGUI/gfx/ResourceManager.hpp"
#include "ToolWindow.hpp"
#include "Widget.hpp"
#include "WidgetTreeRoot.hpp"
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <EssaGUI/gfx/Window.hpp>

#include <cassert>
#include <iostream>

namespace GUI {

Application* s_the {};

Application& Application::the() {
    assert(s_the);
    return *s_the;
}

Application::Application(GUI::Window& wnd)
    : m_window(wnd) {
    assert(!s_the);
    s_the = this;
}

llgl::Event Application::transform_event(Util::Vector2f offset, llgl::Event event) const {
    if (event.type == llgl::Event::Type::MouseButtonPress || event.type == llgl::Event::Type::MouseButtonRelease) {
        event.mouse_button.position.x -= offset.x();
        event.mouse_button.position.y -= offset.y();
    }
    else if (event.type == llgl::Event::Type::MouseMove) {
        event.mouse_move.position.x -= offset.x();
        event.mouse_move.position.y -= offset.y();
    }

    return event;
}

void Application::focus_window(OverlayList::iterator new_focused_it) {
    if (new_focused_it == m_overlays.end())
        return;
    m_focused_overlay = new_focused_it->get();
    auto ptr = std::move(*new_focused_it);
    m_overlays.erase(new_focused_it);
    m_overlays.push_back(std::move(ptr));
}

void Application::handle_event(llgl::Event event) {
    // TODO: Allow user to override closed event

    // Focus window if mouse button pressed
    if (event.type == llgl::Event::Type::MouseButtonPress) {
        m_focused_overlay = nullptr;
        Util::Vector2i position { event.mouse_button.position };
        decltype(m_overlays)::iterator new_focused_it = m_overlays.end();
        for (auto it = m_overlays.end(); it != m_overlays.begin();) {
            auto it2 = --it;
            auto& overlay = **it2;
            if (overlay.full_rect().contains(position)) {
                new_focused_it = it2;
                break;
            }
        }
        focus_window(new_focused_it);
    }

    // Pass events to focused tool window
    if (m_focused_overlay) {
        m_focused_overlay->handle_event(transform_event(m_focused_overlay->position(), event));
        bool scroll_outside_window = event.type == llgl::Event::Type::MouseScroll
            && !m_focused_overlay->full_rect().contains(Util::Vector2i { event.mouse_scroll.position });
        if (!(event.type == llgl::Event::Type::MouseMove || event.type == llgl::Event::Type::MouseButtonRelease || scroll_outside_window))
            return;
    }

    bool should_pass_event_to_main_window = true;

    // Pass mouse moves to all tool windows + capture all scrolls
    for (auto it = m_overlays.rbegin(); it != m_overlays.rend(); it++) {
        auto& overlay = **it;
        if (event.type == llgl::Event::Type::MouseMove) {
            overlay.handle_event(transform_event(overlay.position(), event));
            break;
        }

        bool scroll_on_window = event.type == llgl::Event::Type::MouseScroll
            && overlay.full_rect().contains(Util::Vector2i { event.mouse_scroll.position });

        if (scroll_on_window)
            should_pass_event_to_main_window = false;
    }

    if (should_pass_event_to_main_window)
        WidgetTreeRoot::handle_event(event);
}

void Application::handle_events() {
    llgl::Event event;
    while (window().poll_event(event))
        handle_event(event);
}

void Application::draw(Window& window) {
    WidgetTreeRoot::draw(window);
    for (auto& overlay : m_overlays)
        overlay->draw(window);
}

void Application::draw_notification(Notification const& notification, float y) {
    TextDrawOptions text;
    text.font_size = theme().label_font_size;
    auto text_bounds = window().calculate_text_size(notification.message, font(), text);
    Util::Vector2f text_position { window().size().x() - text_bounds.x() - 20, y + 20 };

    RectangleDrawOptions rs;
    rs.set_border_radius(10);
    switch (notification.level) {
    case NotificationLevel::Error:
        rs.fill_color = { 155, 50, 50, 100 };
        break;
    }

    window().draw_rectangle({ { text_position.x() - 10 + text_position.x(), text_position.y() - 15 + text_position.y() }, { text_bounds.x() + 20, text_bounds.y() + 30 } });
    window().draw_text(notification.message, font(), { window().size().x() - text_bounds.x() - 20, y + 20 });
}

void Application::spawn_notification(Util::UString message, NotificationLevel level) {
    m_notifications.push_back(Notification { .message = std::move(message), .level = level });
}

Overlay& Application::open_overlay_impl(std::unique_ptr<Overlay> overlay) {
    auto overlay_ptr = overlay.get();
    m_next_overlay_position += Util::Vector2f(ToolWindow::TitleBarSize * 2, ToolWindow::TitleBarSize * 2);
    if (m_next_overlay_position.x() > size().x() - ToolWindow::TitleBarSize || m_next_overlay_position.y() > size().y() - ToolWindow::TitleBarSize)
        m_next_overlay_position = { 10, 10 };
    m_overlays.push_back(std::move(overlay));
    return *overlay_ptr;
}

Application::OpenOrFocusResult Application::open_or_focus_tool_window(Util::UString title, std::string id) {
    for (auto it = m_overlays.begin(); it != m_overlays.end(); it++) {
        auto window = it->get();
        if (window->id() == id && dynamic_cast<ToolWindow*>(window)) {
            focus_window(it);
            return { .window = static_cast<ToolWindow*>(window), .opened = false };
        }
    }
    OpenOrFocusResult result = { .window = static_cast<ToolWindow*>(&open_overlay<ToolWindow>(std::move(id))), .opened = true };
    result.window->set_title(std::move(title));
    return result;
}

void Application::open_context_menu(ContextMenu context_menu, Util::Vector2f position) {
    auto& menu = open_overlay<ContextMenuOverlay>(context_menu, position);
    menu.run();
}

void Application::focus_overlay(Overlay& overlay) {
    for (auto it = m_overlays.begin(); it != m_overlays.end(); it++) {
        if (it->get() == &overlay) {
            focus_window(it);
            return;
        }
    }
}

void Application::update() {
    WidgetTreeRoot::update();
    remove_closed_overlays();
    for (auto& overlay : m_overlays)
        overlay->update();
}

void Application::remove_closed_overlays() {
    std::erase_if(m_overlays, [&](auto& wnd) {
        if (wnd->is_closed()) {
            if (wnd->on_close)
                wnd->on_close();
            if (wnd.get() == m_focused_overlay)
                m_focused_overlay = nullptr;
            return true;
        }
        return false;
    });
}

TooltipOverlay& Application::add_tooltip(Tooltip t) {
    auto& overlay = open_overlay<TooltipOverlay>(std::move(t));
    auto& container = overlay.set_main_widget<Container>();
    container.set_layout<HorizontalBoxLayout>();
    return overlay;
}

Theme const& Application::theme() const {
    if (!m_cached_theme) {
        m_cached_theme = Theme {};
        m_cached_theme->load_ini(m_resource_manager.require_lookup_resource(Gfx::ResourceId::asset("Theme.ini"), "")).release_value();
    }
    return *m_cached_theme;
}

}
