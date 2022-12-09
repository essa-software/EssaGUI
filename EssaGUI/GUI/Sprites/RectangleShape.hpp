#pragma once

#include <EssaGUI/Graphics/Window.hpp>
#include <EssaGUI/GUI/Container.hpp>
#include <EssaGUI/GUI/Widget.hpp>
#include <EssaGUI/GUI/Sprites/Sprite.hpp>

namespace GUI {

class RectangleShape : public Sprite {
public:
    virtual void draw(Gfx::Painter&) const override;

    enum class RectangleVertex {
        TOPLEFT,
        TOPRIGHT,
        BOTTOMLEFT,
        BOTTOMRIGHT,
        ALL
    };

    void set_border_radius(RectangleVertex vert, float value);
    float get_border_radius(RectangleVertex vert) const;
    CREATE_VALUE(Util::Color, foreground_color, Util::Colors::Transparent)

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    float m_border_radius[4] { 0 };
};

}
