#include "DragBehavior.hpp"

namespace GUI {

void DragBehavior::start_dragging(Util::Point2i mouse_position) {
    m_drag_start_mouse = mouse_position;
    m_dragging = true;
}

void DragBehavior::on_mouse_move(Util::Point2i new_mouse_position) {
    auto delta = new_mouse_position - m_drag_start_mouse;
    if (m_dragging && delta.length_squared() > 400) {
        m_actually_dragging = true;
    }
    if (m_actually_dragging) {
        m_drag_start_mouse = new_mouse_position;
        m_on_drag(delta);
    }
}

void DragBehavior::stop_dragging() {
    m_dragging = false;
    m_actually_dragging = false;
}

}
