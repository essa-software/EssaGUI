#include "Application.hpp"

#include "ToolWindow.hpp"
#include "Widget.hpp"
#include "WidgetTreeRoot.hpp"
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <EssaGUI/gfx/SFMLWindow.hpp>

#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>

namespace GUI {

Application* s_the {};

Application& Application::the() {
    assert(s_the);
    return *s_the;
}

Application::Application(GUI::SFMLWindow& wnd)
    : WidgetTreeRoot(wnd) {
    assert(!s_the);
    s_the = this;
    font.loadFromFile("../assets/fonts/Xolonium-pn4D.ttf");
    bold_font.loadFromFile("../assets/fonts/XoloniumBold-xKZO.ttf");
    fixed_width_font.loadFromFile("../assets/fonts/SourceCodePro-Regular.otf");
}

sf::Event Application::transform_event(Util::Vector2f offset, sf::Event event) const {
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        event.mouseButton.x -= offset.x();
        event.mouseButton.y -= offset.y();
    }
    else if (event.type == sf::Event::MouseMoved) {
        event.mouseMove.x -= offset.x();
        event.mouseMove.y -= offset.y();
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

void Application::handle_event(sf::Event event) {
    if (event.type == sf::Event::Closed) {
        // TODO: Allow user to override it.
        quit();
        return;
    }

    // Focus window if mouse button pressed
    if (event.type == sf::Event::MouseButtonPressed) {
        m_focused_overlay = nullptr;
        Util::Vector2f position { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };
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
        bool scroll_outside_window = event.type == sf::Event::MouseWheelScrolled
            && !m_focused_overlay->full_rect().contains({ static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y) });
        if (!(event.type == sf::Event::Closed || event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased || scroll_outside_window))
            return;
    }

    bool should_pass_event_to_main_window = true;

    // Pass mouse moves to all tool windows + capture all scrolls
    for (auto it = m_overlays.rbegin(); it != m_overlays.rend(); it++) {
        auto& overlay = **it;
        if (event.type == sf::Event::MouseMoved) {
            overlay.handle_event(transform_event(overlay.position(), event));
            break;
        }

        bool scroll_on_window = event.type == sf::Event::MouseWheelScrolled
            && overlay.full_rect().contains({ static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y) });

        if (scroll_on_window)
            should_pass_event_to_main_window = false;
    }

    if (should_pass_event_to_main_window)
        WidgetTreeRoot::handle_event(event);
}

void Application::handle_events() {
    sf::Event event;
    while (window().pollEvent(event))
        handle_event(event);
}

void Application::draw() {
    WidgetTreeRoot::draw();
    for (auto& overlay : m_overlays)
        overlay->draw();
}

void Application::draw_notification(Notification const& notification, float y) const {
    TextDrawOptions text;
    text.font_size = 15;
    auto text_bounds = window().calculate_text_size(notification.message, font, text);
    Util::Vector2f text_position { window().getSize().x - text_bounds.x() - 20, y + 20 };

    RectangleDrawOptions rs;
    rs.set_border_radius(10);
    switch (notification.level) {
    case NotificationLevel::Error:
        rs.fill_color = { 155, 50, 50, 100 };
        break;
    }

    window().draw_rectangle({ { text_position.x() - 10 + text_position.x(), text_position.y() - 15 + text_position.y() }, { text_bounds.x() + 20, text_bounds.y() + 30 } });
    window().draw_text(notification.message, font, { window().getSize().x - text_bounds.x() - 20, y + 20 });
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

}
