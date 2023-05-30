#include "HostWindow.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/LLGL/OpenGL/Error.hpp>
#include <Essa/LLGL/Window/Event.hpp>

#include <cassert>

namespace GUI {

class FullscreenOverlay : public Overlay {
public:
    explicit FullscreenOverlay(HostWindow& window)
        : Overlay(window, "FullscreenOverlay") {
        set_always_on_bottom(true);
    }

private:
    virtual void handle_event(GUI::Event const& event) override {
        if (auto resize_event = event.get<llgl::Event::WindowResize>()) {
            set_size(resize_event->new_size().cast<int>());
        }
        Overlay::handle_event(event);
    }
};

HostWindow::HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& settings)
    : llgl::Window(size, title, settings)
    , m_next_overlay_position({ 10, 10 + theme().tool_window_title_bar_size }) {
    m_fullscreen_overlay = &open_overlay<FullscreenOverlay>();
    m_fullscreen_overlay->set_size(size.cast<int>());
    llgl::opengl::enable_debug_output();
}

void HostWindow::focus_window(OverlayList::iterator new_focused_it) {
    if (new_focused_it == m_overlays.end())
        return;
    m_focused_overlay = new_focused_it->get();
    if (m_focused_overlay->always_on_bottom())
        return;
    auto ptr = std::move(*new_focused_it);
    m_overlays.erase(new_focused_it);
    m_overlays.push_back(std::move(ptr));
}

void HostWindow::handle_event(GUI::Event const& event) {
    // TODO: Allow user to override closed event

    // Send global events to everyone regardless of the focused overlay
    if (event.target_type() == llgl::EventTargetType::Global) {
        for (auto const& overlay : m_overlays) {
            overlay->handle_event(event.relativized(overlay->position().to_vector()));
        }
    }

    // Run custom event handler; don't pass event to focused overlay if it's accepted
    if (on_event) {
        if (on_event(event) == GUI::Widget::EventHandlerResult::Accepted) {
            return;
        }
    }

    // Don't pass global events to all this mouse related code
    if (event.target_type() == llgl::EventTargetType::Global) {
        return;
    }

    // Focus overlay if mouse button pressed
    if (auto mouse_button = event.get<llgl::Event::MouseButtonPress>()) {
        m_focused_overlay = nullptr;
        decltype(m_overlays)::iterator new_focused_it = m_overlays.end();
        for (auto it = m_overlays.end(); it != m_overlays.begin();) {
            auto it2 = --it;
            auto& overlay = **it2;
            if (!overlay.ignores_events() && overlay.full_rect().contains(mouse_button->local_position())) {
                new_focused_it = it2;
                break;
            }
        }
        focus_window(new_focused_it);
    }

    // Pass all events to focused overlay
    if (m_focused_overlay) {
        // FIXME: Don't pass mouse moves. Currently this breaks moving ToolWindows.
        m_focused_overlay->handle_event(event.relativized(m_focused_overlay->position().to_vector()));
    }

    // Pass mouse events to hovered overlays
    if (event.is_mouse_related()) {
        for (auto it = m_overlays.rbegin(); it != m_overlays.rend(); it++) {
            auto& overlay = **it;
            if (overlay.ignores_events()) {
                continue;
            }
            if (overlay.rect().contains(event.local_mouse_position())) {
                if (m_hovered_overlay != &overlay) {
                    if (m_hovered_overlay) {
                        m_hovered_overlay->handle_event(GUI::Event::MouseLeave());
                    }
                    m_hovered_overlay = &overlay;
                    overlay.handle_event(GUI::Event::MouseEnter());
                }
                if (m_hovered_overlay != m_focused_overlay) {
                    overlay.handle_event(event.relativized(overlay.position().to_vector()));
                }
                break;
            }
        }
    }
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

DBG_DECLARE(GUI_DrawOverlayBounds);

void HostWindow::do_draw() {
    // hacky hacky hacky hacky
    set_active();
    renderer().clear(m_background_color);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_painter.reset();

    Util::Recti viewport { {}, size() };
    m_painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { {}, size().cast<double>() } }, Util::Recti { viewport }));

    for (auto& overlay : m_overlays) {
        overlay->draw(m_painter);
        if (DBG_ENABLED(GUI_DrawOverlayBounds)) {
            using namespace Gfx::Drawing;
            m_painter.draw(Rectangle(overlay->rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Cyan, -1)));
            m_painter.draw(Rectangle(overlay->full_rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Cyan * 0.5, 1)));
            Gfx::Text debug_text(Util::UString(overlay->id()), resource_manager().fixed_width_font());
            debug_text.set_font_size(10);
            debug_text.align(GUI::Align::BottomLeft, overlay->full_rect().cast<float>());
            debug_text.draw(m_painter);
        }
    }

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
    if (!overlay_ptr->ignores_events()) {
        focus_overlay(*overlay_ptr);
    }
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
    remove_closed_overlays();
    for (auto& overlay : m_overlays)
        overlay->update();
}

void HostWindow::remove_closed_overlays() {
    std::erase_if(m_overlays, [&](auto& wnd) {
        if (wnd->is_closed()) {
            if (wnd->on_close) {
                wnd->on_close();
            }
            if (wnd.get() == m_focused_overlay) {
                m_focused_overlay = nullptr;
            }
            if (wnd.get() == m_hovered_overlay) {
                m_hovered_overlay = nullptr;
            }
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

Theme const& HostWindow::theme() const { return Application::the().theme(); }

Gfx::ResourceManager const& HostWindow::resource_manager() const { return Application::the().resource_manager(); }

}
