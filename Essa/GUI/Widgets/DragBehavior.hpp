#pragma once

#include <Essa/GUI/Widgets/Widget.hpp>

namespace GUI {

class DragBehavior {
public:
    using OnDragFunc = std::function<void(Util::Vector2i mouse_moved_by)>;

    explicit DragBehavior(OnDragFunc f)
        : m_on_drag(std::move(f)) {
        assert(m_on_drag);
    }

    void start_dragging(Util::Point2i mouse_position);
    void on_mouse_move(Util::Point2i new_mouse_position);
    void stop_dragging();

    bool is_dragging() const {
        return m_dragging;
    }
    bool is_actually_dragging() const {
        return m_actually_dragging;
    }

private:
    OnDragFunc m_on_drag;
    Util::Point2i m_drag_start_mouse;
    Util::Vector2f m_drag_start_offset;
    bool m_dragging = false;
    bool m_actually_dragging = false;
};

}
