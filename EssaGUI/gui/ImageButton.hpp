#pragma once

#include "Button.hpp"
#include "LLGL/OpenGL/Texture.hpp"

namespace GUI {

class ImageButton : public Button {
public:
    virtual void draw(GUI::Window& window) const override;

    void set_image(llgl::opengl::Texture const* t) { m_texture = t; }

private:
    virtual LengthVector initial_size() const override;

    virtual Theme::ButtonColors default_button_colors() const override { return theme().image_button; }

    llgl::opengl::Texture const* m_texture = nullptr;
};

}
