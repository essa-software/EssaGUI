#pragma once

#include "Container.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"
#include "Widget.hpp"

namespace GUI {

class ScrollableWidget : public Widget {
public:
    explicit ScrollableWidget(Container& parent)
        : Widget(parent) { }

    void set_scroll(float);

protected:
    virtual void handle_event(Event&) override;

    float scroll_area_height() const;
    sf::Vector2f scroll_offset() const;
    void draw_scrollbar(GUI::SFMLWindow&) const;

private:
    virtual float content_height() const = 0;

    float m_scroll {};
};

}
