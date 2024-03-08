#include "Host.hpp"

#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Widgets/MDI/Window.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Is.hpp>

namespace GUI::MDI {

Host::Host() {
    struct BackgroundWindow : public WindowRoot {
        BackgroundWindow(WidgetTreeRoot& window)
            : WindowRoot(window) {
            window.setup("BackgroundWindow", { 1, 1 }, llgl::WindowSettings { .flags = llgl::WindowFlags::Borderless });
        }
    };
    m_background_window = &open_window<BackgroundWindow>().window;
    m_background_window->set_position({});
    m_background_window->set_always_on_bottom(true);

    m_next_window_position = { 10, 10 + theme().tool_window_title_bar_size };
}

void Host::focus_window_it(WindowList::iterator new_focused_it) {
    if (m_focused_window == &**new_focused_it) {
        return;
    }
    if (m_focused_window) {
        m_focused_window->handle_event(llgl::Event::WindowFocusLost());
    }
    if (new_focused_it == m_windows.end()) {
        m_focused_window = nullptr;
        return;
    }

    m_focused_window = new_focused_it->get();
    m_focused_window->handle_event(llgl::Event::WindowFocusGained());

    // Move focused window to top
    if (m_focused_window->always_on_bottom())
        return;
    auto ptr = std::move(*new_focused_it);
    m_windows.erase(new_focused_it);
    m_windows.push_back(std::move(ptr));
}

Widget::EventHandlerResult Host::do_handle_event(GUI::Event const& event) {
    auto base_result = Widget::do_handle_event(event);

    auto transformed_event = event.relativized(raw_position().to_vector());
    if (!is_affected_by_event(transformed_event)) {
        return base_result;
    }

    assert(transformed_event.target_type() != llgl::EventTargetType::Window);

    // Focus overlay if mouse button pressed
    if (auto const* mouse_button = transformed_event.get<llgl::Event::MouseButtonPress>()) {
        decltype(m_windows)::iterator new_focused_it = m_windows.end();
        for (auto it = m_windows.end(); it != m_windows.begin();) {
            auto it2 = --it;
            auto& overlay = **it2;
            if (!overlay.ignores_events() && overlay.full_rect().contains(mouse_button->local_position())) {
                new_focused_it = it2;
                break;
            }
        }
        focus_window_it(new_focused_it);
    }

    // Store focused overlay because it may change in event handler
    auto* focused_overlay = m_focused_window;

    auto gui_to_llgl_event = [](GUI::Event event) {
        return event.visit(
            [](GUI::Event::MouseDoubleClick) -> llgl::Event { ESSA_UNREACHABLE; }, //
            [](GUI::Event::MouseEnter) -> llgl::Event { ESSA_UNREACHABLE; },       //
            [](GUI::Event::MouseLeave) -> llgl::Event { ESSA_UNREACHABLE; },       //
            [](auto event) { return llgl::Event(std::move(event)); }
        );
    };

    // Pass all events to focused overlay
    if (focused_overlay) {
        // FIXME: Don't pass mouse moves. Currently this breaks moving ToolWindows.
        focused_overlay->handle_event(gui_to_llgl_event(transformed_event.relativized(focused_overlay->position().to_vector())));
    }

    // Pass mouse events to hovered overlays
    if (transformed_event.is_mouse_related()) {
        for (auto it = m_windows.rbegin(); it != m_windows.rend(); it++) {
            auto& overlay = **it;
            if (overlay.ignores_events()) {
                continue;
            }
            if (overlay.full_rect().contains(transformed_event.local_mouse_position())) {
                if (m_hovered_window != &overlay) {
                    if (m_hovered_window) {
                        m_hovered_window->handle_event(llgl::Event::WindowMouseLeave());
                    }
                    m_hovered_window = &overlay;
                    overlay.handle_event(llgl::Event::WindowMouseEnter());
                }
                assert(m_hovered_window == &overlay);
                if (&overlay != focused_overlay) {
                    overlay.handle_event(gui_to_llgl_event(transformed_event.relativized(overlay.position().to_vector())));
                }
                break;
            }
        }
    }
    return base_result;
}

DBG_DECLARE(GUI_DrawOverlayBounds);

void Host::draw(Gfx::Painter& painter) const {
    for (auto const& overlay : m_windows) {
        overlay->draw(painter);

        if (DBG_ENABLED(GUI_DrawOverlayBounds)) {
            using namespace Gfx::Drawing;
            painter.draw(Rectangle(overlay->client_rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Cyan, -1)));
            painter.draw(Rectangle(overlay->full_rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Cyan * 0.5, 1)));
            Gfx::Text debug_text(Util::UString(overlay->id()), resource_manager().fixed_width_font());
            debug_text.set_font_size(10);
            debug_text.align(GUI::Align::BottomLeft, overlay->full_rect().cast<float>());
            debug_text.draw(painter);
        }
    }

    theme().renderer().draw_text_editor_border(*this, false, painter);
}

Window& Host::open_window_impl(std::unique_ptr<Window> overlay) {
    auto* overlay_ptr = overlay.get();
    if (overlay_ptr->position() == Util::Point2i {}) {
        overlay_ptr->set_position(m_next_window_position.cast<int>());
        m_next_window_position += Util::Vector2u(theme().tool_window_title_bar_size, theme().tool_window_title_bar_size * 2);
        if (m_next_window_position.x() > raw_size().x() - theme().tool_window_title_bar_size
            || m_next_window_position.y() > raw_size().y() - theme().tool_window_title_bar_size)
            m_next_window_position = { 10, 10 };
    }
    m_windows.push_back(std::move(overlay));
    if (!overlay_ptr->ignores_events()) {
        focus_window(*overlay_ptr);
    }
    return *overlay_ptr;
}

Host::OpenOrFocusResult Host::open_or_focus_window(Util::UString const& title, std::string id) {
    for (auto it = m_windows.begin(); it != m_windows.end(); it++) {
        auto* window = it->get();
        if (window->id() == id) {
            focus_window_it(it);
            return { .window = window, .opened = false };
        }
    }
    OpenOrFocusResult result = { .window = &open_window().window, .opened = true };
    result.window->set_title(title);
    result.window->set_id(id);
    return result;
}

void Host::focus_window(Window& overlay) {
    for (auto it = m_windows.begin(); it != m_windows.end(); it++) {
        if (it->get() == &overlay) {
            focus_window_it(it);
            return;
        }
    }
}

void Host::update() {
    remove_closed_windows();
    for (auto& overlay : m_windows)
        overlay->update();
}

void Host::relayout() {
    m_background_window->set_size(raw_size());
    m_background_window->handle_event(llgl::Event::WindowResize(raw_size().cast<unsigned>()));

    for (auto const& wnd : m_windows) {
        wnd->constrain_position();
    }
}

void Host::remove_closed_windows() {
    std::erase_if(m_windows, [&](auto& wnd) {
        if (wnd->is_closed()) {
            if (wnd->on_close) {
                wnd->on_close();
            }
            if (wnd.get() == m_focused_window) {
                m_focused_window = nullptr;
            }
            if (wnd.get() == m_hovered_window) {
                m_hovered_window = nullptr;
            }
            return true;
        }
        return false;
    });
}

std::vector<DevToolsObject const*> Host::dev_tools_children() const {
    std::vector<DevToolsObject const*> children;
    for (auto const& wnd : m_windows) {
        children.push_back(wnd.get());
    }
    return children;
}

}
