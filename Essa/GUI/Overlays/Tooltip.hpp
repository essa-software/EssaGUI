#pragma once

#include <string>

#include <Essa/GUI/Overlay.hpp>

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

    void set_text(Util::UString t) { m_tooltip.text = std::move(t); }
    Util::UString text() const { return m_tooltip.text; }

private:
    virtual void draw(Gfx::Painter&) override;

    Tooltip m_tooltip;
};

}
