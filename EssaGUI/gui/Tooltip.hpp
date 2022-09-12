#pragma once

#include <string>

#include "Overlay.hpp"

namespace GUI {

class Widget;

struct Tooltip {
    Util::UString text;
    Widget* owner = nullptr;
    Util::Vector2f position;

    Tooltip(Util::UString t, Widget* o, Util::Vector2f p)
        : text(std::move(t))
        , owner(o)
        , position(p) { }
};

class TooltipOverlay : public Overlay {
public:
    TooltipOverlay(Tooltip tooltip, std::string id = "TooltipOverlay")
        : Overlay(std::move(id))
        , m_tooltip(std::move(tooltip)) { }

    virtual Util::Vector2f position() const override { return m_tooltip.position; }

    // TODO (Do we actually need that, this doesn't take any events!)
    virtual Util::Vector2f size() const override { return {}; }

private:
    virtual void draw(GUI::Window&) override;

    Tooltip m_tooltip;
};

}
