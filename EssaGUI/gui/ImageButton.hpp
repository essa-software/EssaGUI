#pragma once

#include "Button.hpp"

namespace GUI {

class ImageButton : public Button {
public:
    ImageButton(Container&, llgl::opengl::Texture const& image);

    virtual void draw(GUI::Window& window) const override;

private:
    virtual LengthVector initial_size() const override;

    virtual Theme::ButtonColors default_button_colors() const override { return theme().image_button; }

    llgl::opengl::Texture const& m_texture;
};

}
