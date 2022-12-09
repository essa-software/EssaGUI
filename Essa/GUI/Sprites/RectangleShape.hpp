#pragma once

#include <Essa/GUI/Graphics/Window.hpp>
#include <Essa/GUI/Sprites/Sprite.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

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
