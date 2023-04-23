#pragma once

#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Vector.hpp>
#include <functional>

namespace GUI {

class Sprite : public Widget {
public:
    virtual void draw(Gfx::Painter&) const override = 0;

    void move_by_vec(Util::Cs::Vector2i const& vec) { set_raw_position(raw_position() + vec); }

    void rotate(Util::Angle angle) { m_rotation = angle; }

    Util::Angle rotation() const { return m_rotation; }

    std::function<void()> on_click;

    CREATE_VALUE(float, outline_thickness, 0)

protected:
    Util::Angle m_rotation;

    virtual EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&) override;
};

}
