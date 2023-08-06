#include "Host.hpp"

#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Widgets/MDI/Window.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Is.hpp>

namespace GUI::MDI {

Host::Host()
    : m_next_overlay_position({ 10, 10 + theme().tool_window_title_bar_size }) {
    m_background_overlay = &open_overlay().overlay;
    m_background_overlay->set_always_on_bottom(true);
}

void Host::focus_window(OverlayList::iterator new_focused_it) {
    if (m_focused_overlay == &**new_focused_it) {
        return;
    }
    if (m_focused_overlay) {
        m_focused_overlay->handle_event(GUI::Event::WindowFocusLost());
    }
    if (new_focused_it == m_overlays.end()) {
        m_focused_overlay = nullptr;
        return;
    }

    m_focused_overlay = new_focused_it->get();
    m_focused_overlay->handle_event(GUI::Event::WindowFocusGained());

    // Move focused window to top
    if (m_focused_overlay->always_on_bottom())
        return;
    auto ptr = std::move(*new_focused_it);
    m_overlays.erase(new_focused_it);
    m_overlays.push_back(std::move(ptr));
}

Widget::EventHandlerResult Host::do_handle_event(GUI::Event const& event) {
    Widget::do_handle_event(event);

    auto transformed_event = event.relativized(raw_position().to_vector());

    // Send global events to everyone regardless of the focused overlay
    if (transformed_event.target_type() == llgl::EventTargetType::Global) {
        for (auto const& overlay : m_overlays) {
            overlay->handle_event(transformed_event.relativized(overlay->position().to_vector()));
        }
    }

    // Don't pass global events to all this mouse related code
    if (transformed_event.target_type() == llgl::EventTargetType::Global) {
        return Widget::EventHandlerResult::NotAccepted;
    }

    // Focus overlay if mouse button pressed
    if (const auto* mouse_button = transformed_event.get<llgl::Event::MouseButtonPress>()) {
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

    // Store focused overlay because it may change in event handler
    auto* focused_overlay = m_focused_overlay;

    // Pass all events to focused overlay
    if (focused_overlay) {
        // FIXME: Don't pass mouse moves. Currently this breaks moving ToolWindows.
        focused_overlay->handle_event(transformed_event.relativized(focused_overlay->position().to_vector()));
    }

    // Pass mouse events to hovered overlays
    if (transformed_event.is_mouse_related()) {
        for (auto it = m_overlays.rbegin(); it != m_overlays.rend(); it++) {
            auto& overlay = **it;
            if (overlay.ignores_events()) {
                continue;
            }
            if (overlay.full_rect().contains(transformed_event.local_mouse_position())) {
                if (m_hovered_overlay != &overlay) {
                    if (m_hovered_overlay) {
                        m_hovered_overlay->handle_event(GUI::Event::MouseLeave());
                    }
                    m_hovered_overlay = &overlay;
                    overlay.handle_event(GUI::Event::MouseEnter());
                }
                assert(m_hovered_overlay == &overlay);
                if (&overlay != focused_overlay) {
                    overlay.handle_event(transformed_event.relativized(overlay.position().to_vector()));
                }
                break;
            }
        }
        return Widget::EventHandlerResult::NotAccepted;
    }
    return is_focused() ? Widget::EventHandlerResult::Accepted : Widget::EventHandlerResult::NotAccepted;
}

DBG_DECLARE(GUI_DrawOverlayBounds);

void Host::draw(Gfx::Painter& painter) const {
    for (const auto& overlay : m_overlays) {
        overlay->draw(painter);

        if (DBG_ENABLED(GUI_DrawOverlayBounds)) {
            using namespace Gfx::Drawing;
            painter.draw(Rectangle(overlay->rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Cyan, -1)));
            painter.draw(Rectangle(overlay->full_rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Cyan * 0.5, 1)));
            Gfx::Text debug_text(Util::UString(overlay->id()), resource_manager().fixed_width_font());
            debug_text.set_font_size(10);
            debug_text.align(GUI::Align::BottomLeft, overlay->full_rect().cast<float>());
            debug_text.draw(painter);
        }
    }

    theme().renderer().draw_text_editor_border(*this, false, painter);
}

Overlay& Host::open_overlay_impl(std::unique_ptr<Overlay> overlay) {
    auto* overlay_ptr = overlay.get();
    m_next_overlay_position += Util::Vector2u(theme().tool_window_title_bar_size * 2, theme().tool_window_title_bar_size * 2);
    if (m_next_overlay_position.x() > raw_size().x() - theme().tool_window_title_bar_size
        || m_next_overlay_position.y() > raw_size().y() - theme().tool_window_title_bar_size)
        m_next_overlay_position = { 10, 10 };
    m_overlays.push_back(std::move(overlay));
    if (!overlay_ptr->ignores_events()) {
        focus_overlay(*overlay_ptr);
    }
    return *overlay_ptr;
}

Host::OpenOrFocusResult Host::open_or_focus_window(Util::UString const& title, std::string id) {
    for (auto it = m_overlays.begin(); it != m_overlays.end(); it++) {
        auto* window = it->get();
        if (window->id() == id && Util::is<Window>(*window)) {
            focus_window(it);
            return { .window = static_cast<Window*>(window), .opened = false };
        }
    }
    OpenOrFocusResult result = { .window = static_cast<Window*>(&open_overlay<Window>(std::move(id))), .opened = true };
    result.window->set_title(title);
    return result;
}

void Host::focus_overlay(Overlay& overlay) {
    for (auto it = m_overlays.begin(); it != m_overlays.end(); it++) {
        if (it->get() == &overlay) {
            focus_window(it);
            return;
        }
    }
}

void Host::update() {
    remove_closed_overlays();
    for (auto& overlay : m_overlays)
        overlay->update();
}

void Host::relayout() {
    m_background_overlay->set_size(raw_size());
    m_background_overlay->handle_event(GUI::Event::WindowResize(raw_size().cast<unsigned>()));
}

void Host::remove_closed_overlays() {
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

}
