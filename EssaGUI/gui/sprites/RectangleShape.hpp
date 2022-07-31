#pragma once

#include "EssaGUI/gfx/Window.hpp"
#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/Widget.hpp"
#include "EssaGUI/gui/sprites/Sprite.hpp"

namespace GUI {

class RectangleShape : public Sprite {
public:
    explicit RectangleShape(Container& c)
        : Sprite(c) { }

    virtual void draw(Window& window) const override;

    enum class RectangleVertex {
        TOPLEFT,
        TOPRIGHT,
        BOTTOMLEFT,
        BOTTOMRIGHT,
        ALL
    };

    void set_border_radius(RectangleVertex vert, float value);
    float get_border_radius(RectangleVertex vert) const;

private:
    float m_border_radius[4] { 0 };
};

}
