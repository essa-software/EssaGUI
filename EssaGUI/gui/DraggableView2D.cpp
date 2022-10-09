#include "DraggableView2D.hpp"

namespace GUI {

llgl::Transform DraggableView2D::transform() const {
    return llgl::Transform {}
        .translate(Util::Vector3f { raw_size() / 2.f, 0 })
        .scale(m_zoom)
        .translate(Util::Vector3f { m_offset, 0 });
}

void DraggableView2D::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::MouseScroll) {
        if (event.event().mouse_scroll.delta > 0) {
            m_zoom *= 2;
            auto delta = (Util::Vector2f { event.mouse_position() } - raw_size() / 2.f) / m_zoom;
            m_offset -= delta;
        }
        else {
            m_zoom /= 2;
            auto delta = (Util::Vector2f { event.mouse_position() } - raw_size() / 2.f) / m_zoom;
            m_offset += delta / 2.f;
        }
    }
    else if (event.type() == llgl::Event::Type::MouseButtonPress) {
        m_drag_start_mouse = event.mouse_position();
        m_drag_start_offset = m_offset;
        m_dragging = true;
    }
    else if (event.type() == llgl::Event::Type::MouseButtonRelease) {
        m_dragging = false;
    }
    else if (event.type() == llgl::Event::Type::MouseMove) {
        if (m_dragging) {
            auto delta = event.mouse_position() - m_drag_start_mouse;
            m_offset = m_drag_start_offset + Util::Vector2f { delta } / m_zoom;
        }
    }
}

}
