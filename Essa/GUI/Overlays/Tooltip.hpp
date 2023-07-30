#pragma once

#include "Essa/GUI/Widgets/MDI/Overlay.hpp"
#include <string>

#include <Essa/GUI/Overlay.hpp>

namespace GUI {

class Widget;

struct Tooltip {
    Util::UString text;
    Util::Point2i position;

    Tooltip(Util::UString t, Util::Point2i p)
        : text(std::move(t))
        , position(p) { }
};

class TooltipOverlay : public Overlay {
public:
    TooltipOverlay(MDI::Host& window, Tooltip tooltip, std::string id = "TooltipOverlay")
        : Overlay(window, std::move(id))
        , m_tooltip(std::move(tooltip)) {
        set_ignores_events(true);
    }

    virtual Util::Point2i position() const override { return m_tooltip.position; }

    // TODO (Do we actually need that, this doesn't take any events!)
    virtual Util::Size2i size() const override { return {}; }

    void set_position(Util::Point2i p) { m_tooltip.position = p; }

    void set_text(Util::UString t) { m_tooltip.text = std::move(t); }
    Util::UString text() const { return m_tooltip.text; }

private:
    virtual void draw(Gfx::Painter&) override;

    Tooltip m_tooltip;
};

}
