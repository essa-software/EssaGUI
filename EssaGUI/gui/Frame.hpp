#pragma once

#include "Container.hpp"

namespace GUI {

class Frame : public Container {
public:
    explicit Frame(Container& c)
        : Container(c) { }

    static constexpr float BorderRadius = 20;

private:
    virtual void draw(sf::RenderWindow&) const override;
    virtual float intrinsic_padding() const override { return BorderRadius; }
    virtual bool isolated_focus() const override { return true; }
};

}
