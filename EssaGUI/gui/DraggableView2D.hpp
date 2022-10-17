#pragma once

#include "Widget.hpp"
#include <LLGL/Core/Transform.hpp>
#include <LLGL/Window/Mouse.hpp>

namespace GUI {

class DraggableView2D : public Widget {
public:
    llgl::Transform transform() const;
    llgl::Transform inverse_transform() const;

    float zoom() const { return m_zoom; }
    auto offset() const { return m_offset; }
    void set_zoom(float z) { m_zoom = z; }
    Util::Rectf visible_area() const;
    Util::Vector2f screen_to_world(Util::Vector2i) const;
    Util::Vector2i world_to_screen(Util::Vector2f) const;
    bool dragging() const { return m_dragging; }
    bool actually_dragging() const { return m_actually_dragging; }
    void set_pan_button(llgl::MouseButton button) { m_pan_button = button; }

protected:
    virtual void handle_event(Event&) override;

private:
    float m_zoom = 1;
    Util::Vector2f m_offset;

    Util::Vector2i m_drag_start_mouse;
    Util::Vector2f m_drag_start_offset;
    bool m_dragging = false;
    bool m_actually_dragging = false;
    llgl::MouseButton m_pan_button = llgl::MouseButton::Middle;
};

}
