#include "HostWindow.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/LLGL/OpenGL/Error.hpp>
#include <Essa/LLGL/Window/Event.hpp>

#include <cassert>

namespace GUI {

HostWindow::HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& settings)
    : llgl::Window(size, title, settings) {
    llgl::opengl::enable_debug_output();
}

void HostWindow::focus_window(OverlayList::iterator new_focused_it) {
    if (new_focused_it == m_overlays.end())
        return;
    m_focused_overlay = new_focused_it->get();
    auto ptr = std::move(*new_focused_it);
    m_overlays.erase(new_focused_it);
    m_overlays.push_back(std::move(ptr));
}

void HostWindow::handle_event(GUI::Event const& event) {
    // TODO: Allow user to override closed event

    if (event.target_type() == llgl::EventTargetType::Global) {
        for (auto const& overlay : m_overlays) {
            overlay->handle_event(event.relativized(overlay->position().to_vector()));
        }
        WidgetTreeRoot::handle_event(event);
        return;
    }

    // Focus window if mouse button pressed
    if (auto mouse_button = event.get<llgl::Event::MouseButtonPress>()) {
        m_focused_overlay = nullptr;
        decltype(m_overlays)::iterator new_focused_it = m_overlays.end();
        for (auto it = m_overlays.end(); it != m_overlays.begin();) {
            auto it2 = --it;
            auto& overlay = **it2;
            if (overlay.full_rect().contains(mouse_button->local_position())) {
                new_focused_it = it2;
                break;
            }
        }
        focus_window(new_focused_it);
    }

    // Pass events to focused tool window
    if (m_focused_overlay) {
        m_focused_overlay->handle_event(event.relativized(m_focused_overlay->position().to_vector()));
        bool scroll_outside_window = event.is<llgl::Event::MouseScroll>()
            && !m_focused_overlay->full_rect().contains(event.get<llgl::Event::MouseScroll>()->local_position());
        if (!(event.is<llgl::Event::MouseMove>() || event.is<llgl::Event::MouseButtonRelease>() || scroll_outside_window))
            return;
    }

    bool should_pass_event_to_main_window = true;

    // Pass mouse moves to all tool windows + capture all scrolls
    for (auto it = m_overlays.rbegin(); it != m_overlays.rend(); it++) {
        auto& overlay = **it;
        if (event.is<llgl::Event::MouseMove>()) {
            overlay.handle_event(event.relativized(overlay.position().to_vector()));
            break;
        }

        bool scroll_on_window
            = event.is<llgl::Event::MouseScroll>() && overlay.full_rect().contains(event.get<llgl::Event::MouseScroll>()->local_position());

        if (scroll_on_window)
            should_pass_event_to_main_window = false;
    }

    if (on_event) {
        if (on_event(event) == GUI::Widget::EventHandlerResult::Accepted) {
            return;
        }
    }

    if (should_pass_event_to_main_window)
        WidgetTreeRoot::handle_event(event);
}

void HostWindow::handle_events() {
    while (true) {
        auto event = poll_event();
        if (!event) {
            break;
        }
        handle_event(*event);
    }
}

void HostWindow::do_draw() {
    // hacky hacky hacky hacky
    set_active();
    renderer().clear(m_background_color);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_painter.reset();

    Util::Recti viewport { {}, size() };
    m_painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { {}, size().cast<double>() } }, Util::Recti { viewport }));

    WidgetTreeRoot::draw(m_painter);
    for (auto& overlay : m_overlays)
        overlay->draw(m_painter);

    m_painter.render();
    display();
}

Overlay& HostWindow::open_overlay_impl(std::unique_ptr<Overlay> overlay) {
    auto overlay_ptr = overlay.get();
    m_next_overlay_position += Util::Vector2f(theme().tool_window_title_bar_size * 2, theme().tool_window_title_bar_size * 2);
    if (m_next_overlay_position.x() > size().x() - theme().tool_window_title_bar_size
        || m_next_overlay_position.y() > size().y() - theme().tool_window_title_bar_size)
        m_next_overlay_position = { 10, 10 };
    m_overlays.push_back(std::move(overlay));
    focus_overlay(*overlay_ptr);
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

void HostWindow::open_context_menu(ContextMenu context_menu, Util::Point2i position) {
    auto& menu = open_overlay<ContextMenuOverlay>(context_menu, position);
    menu.show_modal();
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
