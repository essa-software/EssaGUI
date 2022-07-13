#pragma once

#include <SFML/System.hpp>
#include <string>

#include "Overlay.hpp"

namespace GUI {

class Widget;

struct Tooltip {
    Util::UString text;
    Widget* owner = nullptr;
    sf::Vector2f position;

    Tooltip(Util::UString t, Widget* o, sf::Vector2f p)
        : text(std::move(t))
        , owner(o)
        , position(p) { }
};

class TooltipOverlay : public Overlay {
public:
    TooltipOverlay(GUI::SFMLWindow& wnd, Tooltip tooltip, std::string id = "TooltipOverlay")
        : Overlay(wnd, std::move(id))
        , m_tooltip(std::move(tooltip)) { }

    virtual sf::Vector2f position() const override { return m_tooltip.position; }

    // TODO (Do we actually need that, this doesn't take any events!)
    virtual sf::Vector2f size() const override { return {}; }

private:
    virtual void draw() override;

    Tooltip m_tooltip;
};

}
