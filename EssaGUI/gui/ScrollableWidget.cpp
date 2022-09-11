#include "ScrollableWidget.hpp"

namespace GUI {

void ScrollableWidget::handle_event(Event& event) {
    switch (event.type()) {
    case llgl::Event::Type::MouseScroll: {
        if (!is_hover()) {
            // TODO: Check wheel
            break;
        }
        event.set_handled();
        if (content_height() < scroll_area_height())
            break;
        m_scroll -= event.event().mouse_scroll.delta * 60;
        if (m_scroll < 0)
            m_scroll = 0;
        double bottom_content = content_height() - scroll_area_height();
        if (m_scroll > bottom_content)
            m_scroll = bottom_content;
    } break;
    default:
        break;
    }
}

Util::Vector2f ScrollableWidget::scroll_offset() const {
    return { 0, -m_scroll };
}

void ScrollableWidget::set_scroll(float scroll) {
    m_scroll = scroll;
}

void ScrollableWidget::draw_scrollbar(GUI::Window& window) const {
    // "Scrollbar"
    float scroll_area_size = this->scroll_area_height();
    float content_size = this->content_height();
    if (content_size > scroll_area_size) {
        RectangleDrawOptions scrollbar;
        scrollbar.fill_color = Util::Color { 200, 200, 200 };
        window.draw_rectangle({ { raw_size().x() - 5, m_scroll * scroll_area_size / content_size + 2 }, { 3, scroll_area_size / content_size * scroll_area_size - 4 } }, scrollbar);
    }
}

float ScrollableWidget::scroll_area_height() const {
    return raw_size().y();
}

}
