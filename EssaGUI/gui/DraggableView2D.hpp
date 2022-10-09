#pragma once

#include "Widget.hpp"
#include <LLGL/Core/Transform.hpp>

namespace GUI {

class DraggableView2D : public Widget {
public:
    llgl::Transform transform() const;

    float zoom() const { return m_zoom; }
    auto offset() const { return m_offset; }
    Util::Rectf visible_area() const;

private:
    virtual void handle_event(Event&) override;

    float m_zoom = 1;
    Util::Vector2f m_offset;

    Util::Vector2i m_drag_start_mouse;
    Util::Vector2f m_drag_start_offset;
    bool m_dragging = false;
};

}
