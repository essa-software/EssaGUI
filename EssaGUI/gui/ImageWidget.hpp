#pragma once

#include "Widget.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

class ImageWidget : public Widget {
public:
    explicit ImageWidget(Container& c)
        : Widget(c) { }

    void set_image(sf::Texture const* image) {
        m_image = image;
    }

private:
    virtual void draw(GUI::SFMLWindow& window) const override;

    sf::Texture const* m_image = nullptr;
};

}
