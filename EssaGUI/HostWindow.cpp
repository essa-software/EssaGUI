#include "HostWindow.hpp"

#include <EssaGUI/Graphics/Text.hpp>
#include <EssaGUI/Application.hpp>
#include <EssaGUI/Widgets/Container.hpp>

#include <cassert>

namespace GUI {

HostWindow::HostWindow(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& settings)
    : m_window(size, title, settings) {
}

llgl::Event HostWindow::transform_event(Util::Vector2f offset, llgl::Event event) const {
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

void HostWindow::focus_window(OverlayList::iterator new_focused_it) {
    if (new_focused_it == m_overlays.end())
        return;
    m_focused_overlay = new_focused_it->get();
    auto ptr = std::move(*new_focused_it);
    m_overlays.erase(new_focused_it);
    m_overlays.push_back(std::move(ptr));
}

void HostWindow::handle_event(llgl::Event event) {
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

void HostWindow::handle_events() {
    llgl::Event event;
    while (window().poll_event(event))
        handle_event(event);
}

void HostWindow::do_draw() {
    // hacky hacky hacky hacky
    window().set_active();
    window().clear();
    glClear(GL_DEPTH_BUFFER_BIT);
    m_painter.reset();

    Util::Rectf viewport { {}, size() };
    m_painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { {}, Util::Vector2d { size() } } }, Util::Recti { viewport }));

    WidgetTreeRoot::draw(m_painter);
    for (auto& overlay : m_overlays)
        overlay->draw(m_painter);

    m_painter.render();
    window().display();
}

void HostWindow::draw_notification(Notification const& notification, float y) {
    Gfx::Text text { notification.message, GUI::Application::the().font() };
    text.set_font_size(theme().label_font_size);
    auto text_bounds = text.calculate_text_size();
    Util::Vector2f text_position { window().size().x() - text_bounds.x() - 20, y + 20 };
    text.set_position(text_position);

    Gfx::RectangleDrawOptions rs;
    rs.set_border_radius(10);
    switch (notification.level) {
    case NotificationLevel::Error:
        rs.fill_color = { 155, 50, 50, 100 };
        break;
    }

    m_painter.draw_rectangle({ { text_position.x() - 10 + text_position.x(), text_position.y() - 15 + text_position.y() }, { text_bounds.x() + 20, text_bounds.y() + 30 } });
    text.draw(m_painter);
}

void HostWindow::spawn_notification(Util::UString message, NotificationLevel level) {
    m_notifications.push_back(Notification { .message = std::move(message), .level = level });
}

Overlay& HostWindow::open_overlay_impl(std::unique_ptr<Overlay> overlay) {
    auto overlay_ptr = overlay.get();
    m_next_overlay_position += Util::Vector2f(ToolWindow::TitleBarSize * 2, ToolWindow::TitleBarSize * 2);
    if (m_next_overlay_position.x() > size().x() - ToolWindow::TitleBarSize || m_next_overlay_position.y() > size().y() - ToolWindow::TitleBarSize)
        m_next_overlay_position = { 10, 10 };
    m_overlays.push_back(std::move(overlay));
    return *overlay_ptr;
}

HostWindow::OpenOrFocusResult HostWindow::open_or_focus_tool_window(Util::UString title, std::string id) {
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

void HostWindow::open_context_menu(ContextMenu context_menu, Util::Vector2f position) {
    auto& menu = open_overlay<ContextMenuOverlay>(context_menu, position);
    menu.run();
}

void HostWindow::focus_overlay(Overlay& overlay) {
    for (auto it = m_overlays.begin(); it != m_overlays.end(); it++) {
        if (it->get() == &overlay) {
            focus_window(it);
            return;
        }
    }
}

void HostWindow::update() {
    WidgetTreeRoot::update();
    remove_closed_overlays();
    for (auto& overlay : m_overlays)
        overlay->update();
}

void HostWindow::remove_closed_overlays() {
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

TooltipOverlay& HostWindow::add_tooltip(Tooltip t) {
    auto& overlay = open_overlay<TooltipOverlay>(std::move(t));
    auto& container = overlay.set_main_widget<Container>();
    container.set_layout<HorizontalBoxLayout>();
    return overlay;
}

}
