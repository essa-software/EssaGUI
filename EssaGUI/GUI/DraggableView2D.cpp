#include "DraggableView2D.hpp"

namespace GUI {

llgl::Transform DraggableView2D::transform() const {
    return llgl::Transform {}
        .translate(Util::Vector3f { raw_size() / 2.f, 0 })
        .scale(m_zoom)
        .translate(Util::Vector3f { -m_offset, 0 });
}

llgl::Transform DraggableView2D::inverse_transform() const {
    return llgl::Transform {}
        .translate(Util::Vector3f { m_offset, 0 })
        .scale(1 / m_zoom)
        .translate(Util::Vector3f { -raw_size() / 2.f, 0 });
}

Util::Rectf DraggableView2D::visible_area() const {
    return {
        m_offset.x() - raw_size().x() / 2.f / zoom(),
        m_offset.y() - raw_size().y() / 2.f / zoom(),
        raw_size().x() / zoom(),
        raw_size().y() / zoom()
    };
}

Util::Vector2f DraggableView2D::screen_to_world(Util::Vector2i screen) const {
    return Util::Vector2f { inverse_transform().transform_point(Util::Vector3f { screen, 1 }) };
}

Util::Vector2i DraggableView2D::world_to_screen(Util::Vector2f world) const {
    return Util::Vector2i { transform().transform_point(Util::Vector3f { world, 1 }) };
}

void DraggableView2D::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::MouseScroll) {
        if (event.event().mouse_scroll.delta > 0) {
            m_zoom *= 2;
            auto delta = (Util::Vector2f { event.mouse_position() } - raw_size() / 2.f) / m_zoom;
            m_offset += delta;
        }
        else {
            m_zoom /= 2;
            auto delta = (Util::Vector2f { event.mouse_position() } - raw_size() / 2.f) / m_zoom;
            m_offset -= delta / 2.f;
        }
    }
    else if (event.type() == llgl::Event::Type::MouseButtonPress) {
        if (!is_hover()) {
            return;
        }
        if (event.event().mouse_button.button != m_pan_button) {
            return;
        }
        m_drag_start_mouse = event.mouse_position();
        m_drag_start_offset = m_offset;
        m_dragging = true;
    }
    else if (event.type() == llgl::Event::Type::MouseButtonRelease) {
        if (event.event().mouse_button.button != m_pan_button) {
            return;
        }
        m_dragging = false;
        m_actually_dragging = false;
    }
    else if (event.type() == llgl::Event::Type::MouseMove) {
        auto delta = event.mouse_position() - m_drag_start_mouse;
        if (m_dragging && delta.length_squared() > 400) {
            m_actually_dragging = true;
        }
        if (m_actually_dragging) {
            m_offset = m_drag_start_offset - Util::Vector2f { delta } / m_zoom;
        }
    }
}

}
