#pragma once

#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Widget.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Vector.hpp>
#include <functional>

namespace GUI {

class Sprite : public Widget {
public:
    virtual void draw(Window& window) const override = 0;

    void move_by_vec(Util::Vector2f const& vec) { set_raw_position(raw_position() + vec); }

    void rotate(Util::Angle angle) { m_rotation = angle; }

    Util::Angle rotation() const { return m_rotation; }

    std::function<void()> on_click;

    CREATE_VALUE(float, outline_thickness, 0)

protected:
    Util::Angle m_rotation;

    virtual void handle_event(Event&) override;
};

}
