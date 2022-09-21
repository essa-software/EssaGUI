#pragma once

#include <string>

#include "Overlay.hpp"

namespace GUI {

class Widget;

struct Tooltip {
    Util::UString text;
    Util::Vector2f position;

    Tooltip(Util::UString t, Util::Vector2f p)
        : text(std::move(t))
        , position(p) { }
};

class TooltipOverlay : public Overlay {
public:
    TooltipOverlay(HostWindow& window, Tooltip tooltip, std::string id = "TooltipOverlay")
        : Overlay(window, std::move(id))
        , m_tooltip(std::move(tooltip)) { }

    virtual Util::Vector2f position() const override { return m_tooltip.position; }

    // TODO (Do we actually need that, this doesn't take any events!)
    virtual Util::Vector2f size() const override { return {}; }

    void set_position(Util::Vector2f p) { m_tooltip.position = p; }

private:
    virtual void draw(GUI::Window&) override;

    Tooltip m_tooltip;
};

}
