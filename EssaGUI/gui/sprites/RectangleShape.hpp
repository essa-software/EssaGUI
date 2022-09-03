#pragma once

#include "EssaGUI/gfx/Window.hpp"
#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/Widget.hpp"
#include "EssaGUI/gui/sprites/Sprite.hpp"

namespace GUI {

class RectangleShape : public Sprite {
public:
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

    Util::Color background_color() const { return m_background_color; }
    void set_background_color(Util::Color color) { m_background_color = color; }

    Util::Color foreground_color() const { return m_foreground_color; }
    void set_foreground_color(Util::Color color) { m_foreground_color = color; }

private:
    float m_border_radius[4] { 0 };
    Util::Color m_background_color;
    Util::Color m_foreground_color;
};

}
