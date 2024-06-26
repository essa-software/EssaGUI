#pragma once

#include <Essa/GUI/Widgets/DragBehavior.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>

namespace GUI {

class DraggableView2D : public Widget {
public:
    DraggableView2D();

    llgl::Transform transform() const;
    llgl::Transform inverse_transform() const;

    float zoom() const {
        return m_zoom;
    }
    void set_zoom(float z) {
        m_zoom = z;
    }
    auto offset() const {
        return m_offset;
    }
    void set_offset(Util::Vector2f off) {
        m_offset = off;
    }
    Util::Rectf visible_area() const;
    Util::Point2f screen_to_world(Util::Point2i const&) const;
    Util::Point2i world_to_screen(Util::Point2f const&) const;
    bool dragging() const {
        return m_drag_behavior.is_dragging();
    }
    bool actually_dragging() const {
        return m_drag_behavior.is_actually_dragging();
    }
    void set_pan_button(llgl::MouseButton button) {
        m_pan_button = button;
    }
    void set_zoom_enabled(bool enabled) {
        m_zoom_enabled = enabled;
    }

protected:
    virtual EventHandlerResult on_mouse_scroll(Event::MouseScroll const& event) override;
    virtual EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const& event) override;
    virtual EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const& event) override;
    virtual EventHandlerResult on_mouse_move(Event::MouseMove const& event) override;
    virtual void update() override;

private:
    void set_zoom_with_animation(float new_zoom, Util::Point2i const& origin);

    float m_zoom = 1;
    float m_zoom_speed = 0.f;
    bool m_zoom_enabled = true;
    Util::Vector2f m_offset;

    llgl::MouseButton m_pan_button = llgl::MouseButton::Middle;
    DragBehavior m_drag_behavior;
};

}
