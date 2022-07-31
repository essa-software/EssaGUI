#pragma once

#include "EssaGUI/gfx/Window.hpp"
#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/Widget.hpp"
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Vector.hpp>
#include <functional>
namespace GUI {

class Sprite : public Widget {
public:
    explicit Sprite(Container& c)
        : Widget(c) { }

    virtual void draw(Window& window) const override = 0;

    void move_by_vec(Util::Vector2f const& vec) { set_raw_position(position() + vec); }

    void rotate(Util::Angle angle) { m_rotation = angle; }

    Util::Angle rotation() const { return m_rotation; }

    std::function<void()> on_click;

    void set_outline_thickness(float thickness) { m_outline_thickness = thickness; }

    float get_outline_thickness() const { return m_outline_thickness; }

protected:
    Util::Angle m_rotation;
    float m_outline_thickness = 0;

    virtual void handle_event(Event&) override;
};

}
