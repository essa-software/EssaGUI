#include "DraggableView2D.hpp"

#include <Essa/LLGL/Window/Mouse.hpp>

namespace GUI {

DraggableView2D::DraggableView2D()
    : m_drag_behavior([&](Util::Vector2i delta) { m_offset -= delta.cast<float>() / m_zoom; }) { }

llgl::Transform DraggableView2D::transform() const {
    return llgl::Transform {}.translate_2d((raw_size().cast<float>().to_vector() / 2.f).rounded()).scale(m_zoom).translate_2d(-m_offset);
}

llgl::Transform DraggableView2D::inverse_transform() const {
    return llgl::Transform {}
        .translate_2d(m_offset)
        .scale(1 / m_zoom)
        .translate_2d(-(raw_size().cast<float>().to_vector() / 2.f).rounded());
}

Util::Rectf DraggableView2D::visible_area() const {
    return { m_offset.x() - raw_size().x() / 2.f / zoom(), m_offset.y() - raw_size().y() / 2.f / zoom(), raw_size().x() / zoom(),
             raw_size().y() / zoom() };
}

Util::Point2f DraggableView2D::screen_to_world(Util::Point2i const& screen) const {
    return inverse_transform().transform_point_2d(screen.cast<float>());
}

Util::Point2i DraggableView2D::world_to_screen(Util::Point2f const& world) const {
    return transform().transform_point_2d(world).cast<int>();
}

void DraggableView2D::set_zoom_with_animation(float new_zoom, Util::Point2i const& origin) {
    auto worigin = screen_to_world(origin).to_vector();
    float fac = new_zoom / m_zoom;
    auto new_offset = (m_offset - worigin) / fac + worigin;

    m_zoom = new_zoom;
    m_offset = new_offset;
}

Widget::EventHandlerResult DraggableView2D::on_mouse_scroll(Event::MouseScroll const& event) {
    if (m_zoom_enabled) {
        if (event.delta() > 0) {
            if (m_zoom_speed < 0) {
                m_zoom_speed = 0;
            }
            m_zoom_speed += 0.15f;
        }
        else {
            if (m_zoom_speed > 0) {
                m_zoom_speed = 0;
            }
            m_zoom_speed -= 0.15f;
        }
    }
    return Widget::EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult DraggableView2D::on_mouse_button_press(Event::MouseButtonPress const& event) {
    if (event.button() != m_pan_button) {
        return Widget::EventHandlerResult::NotAccepted;
    }
    m_drag_behavior.start_dragging(event.local_position());
    return EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult DraggableView2D::on_mouse_button_release(Event::MouseButtonRelease const& event) {
    if (event.button() != m_pan_button) {
        return Widget::EventHandlerResult::NotAccepted;
    }
    m_drag_behavior.stop_dragging();
    return Widget::EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult DraggableView2D::on_mouse_move(Event::MouseMove const& event) {
    m_drag_behavior.on_mouse_move(event.local_position());
    return EventHandlerResult::NotAccepted;
}

void DraggableView2D::update() {
    if (m_zoom_speed != 0) {
        set_zoom_with_animation(m_zoom * (1 + m_zoom_speed), (llgl::mouse_position() - this->raw_position()).to_point());
    }
    m_zoom_speed *= 0.7f;
    if (std::abs(m_zoom_speed) < 0.001f) {
        m_zoom_speed = 0.f;
    }
}

}
