#pragma once

#include "Button.hpp"
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/OpenGL/Texture.hpp>

namespace GUI {

class ImageButton : public Button {
public:
    virtual void draw(Gfx::Painter& window) const override;

    CREATE_VALUE(llgl::Texture const*, image, nullptr)

private:
    virtual LengthVector initial_size() const override;
    virtual Theme::ButtonColors default_button_colors() const override { return theme().image_button; }
};

}
