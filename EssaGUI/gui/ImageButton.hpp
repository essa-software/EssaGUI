#pragma once

#include "Button.hpp"
#include "EssaGUI/gui/Widget.hpp"
#include "LLGL/OpenGL/Texture.hpp"

namespace GUI {

class ImageButton : public Button {
public:
    virtual void draw(GUI::Window& window) const override;

    CREATE_VALUE(llgl::Texture const*, image, nullptr)

private:
    virtual LengthVector initial_size() const override;
    virtual Theme::ButtonColors default_button_colors() const override { return theme().image_button; }
};

}
