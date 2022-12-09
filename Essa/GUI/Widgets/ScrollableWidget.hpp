#pragma once

#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/GUI/Graphics/Window.hpp>

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
    // The rect that the scroll is applied to. Scrollbars are drawn on edges
    // of that rect, and scroll_area_size() is related to this rect. By default,
    // the whole widget is scrollable.
    virtual Util::Rectf scrollable_rect() const { return local_rect(); }

    // Size of the content that is scrolled over in the scrollable rect. If
    // content size overflows scrollable rect, scrollbars will appear.
    virtual Util::Vector2f content_size() const = 0;

    Util::Vector2f m_scroll {};
};

}
