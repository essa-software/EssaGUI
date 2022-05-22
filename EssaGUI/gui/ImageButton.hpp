#pragma once

#include "Button.hpp"

namespace GUI {

class ImageButton : public Button {
public:
    ImageButton(Container&, sf::Image);

    virtual void draw(sf::RenderWindow& window) const override;

private:
    virtual LengthVector initial_size() const override;

    virtual Theme::ButtonColors default_button_colors() const override { return theme().image_button; }

    sf::Texture m_texture;
};

}
