#pragma once

#include "Container.hpp"
#include "Widget.hpp"
#include <EssaGUI/gfx/Window.hpp>

namespace GUI {

class ScrollableWidget : public Widget {
public:
    void set_scroll(Util::Vector2f);
    void set_scroll_x(float x) { m_scroll.x() = x; }
    void set_scroll_y(float y) { m_scroll.y() = y; }
    Util::Vector2f scroll() const { return m_scroll; }

protected:
    virtual void handle_event(Event&) override;

    Util::Vector2f scroll_area_size() const;
    Util::Vector2f scroll_offset() const;
    void draw_scrollbar(Gfx::Painter&) const;
    void scroll_to_bottom();

private:
    virtual Util::Vector2f content_size() const = 0;

    Util::Vector2f m_scroll {};
};

}
