#include "ScrollableWidget.hpp"

#include <EssaUtil/Orientation.hpp>

namespace GUI {

Widget::EventHandlerResult ScrollableWidget::on_mouse_scroll(Event::MouseScroll const& event) {
    auto orientation = llgl::is_shift_pressed() ? Util::Orientation::Horizontal : Util::Orientation::Vertical;

    auto content_component = content_size().main(orientation);
    auto scroll_area_component = scroll_area_size().main(orientation);
    auto scroll_component = orientation == Util::Orientation::Horizontal ? m_scroll.x() : m_scroll.y();

    if (content_component < scroll_area_component) {
        return Widget::EventHandlerResult::NotAccepted;
    }

    scroll_component -= event.delta() * 60;
    if (scroll_component < 0)
        scroll_component = 0;
    int bottom_content = content_component - scroll_area_component;
    if (scroll_component > bottom_content)
        scroll_component = bottom_content;

    if (orientation == Util::Orientation::Horizontal) {
        m_scroll.set_x(scroll_component);
    }
    else {
        m_scroll.set_y(scroll_component);
    }

    clamp_scrollbars();

    if (on_scroll) {
        on_scroll();
    }

    return Widget::EventHandlerResult::Accepted;
}

void ScrollableWidget::clamp_scrollbars() {
    auto content_size = this->content_size();
    auto scroll_area_size = this->scroll_area_size();
    if (m_scroll.x() > content_size.x() - scroll_area_size.x()) {
        m_scroll.set_x(content_size.x() - scroll_area_size.x());
    }
    if (m_scroll.x() < 0) {
        m_scroll.set_x(0);
    }
    if (m_scroll.y() > content_size.y() - scroll_area_size.y()) {
        m_scroll.set_y(content_size.y() - scroll_area_size.y());
    }
    if (m_scroll.y() < 0) {
        m_scroll.set_y(0);
    }
}

Util::Vector2i ScrollableWidget::scroll_offset() const {
    return -m_scroll;
}

void ScrollableWidget::set_scroll(Util::Vector2i scroll) {
    m_scroll = scroll;
    clamp_scrollbars();
}

void ScrollableWidget::draw_scrollbar(Gfx::Painter& window) const {
    auto scrollable_rect = this->scrollable_rect();
    if (m_x_scrollbar_visible) {
        float scroll_area_size = this->scroll_area_size().x();
        float content_size = this->content_size().x();
        if (content_size > scroll_area_size) {
            Gfx::RectangleDrawOptions scrollbar;
            scrollbar.fill_color = Util::Color { 200, 200, 200 };
            window.deprecated_draw_rectangle(
                {
                    { scrollable_rect.position().x() + m_scroll.x() * scroll_area_size / content_size + 2,
                      scrollable_rect.position().y() + scrollable_rect.size().y() - 5 },
                    { scroll_area_size / content_size * scroll_area_size - 4, 3 },
                },
                scrollbar
            );
        }
    }
    if (m_y_scrollbar_visible) {
        float scroll_area_size = this->scroll_area_size().y();
        float content_size = this->content_size().y();
        if (content_size == 0) {
            return;
        }
        if (content_size > scroll_area_size) {
            Gfx::RectangleDrawOptions scrollbar;
            scrollbar.fill_color = Util::Color { 200, 200, 200 };
            window.deprecated_draw_rectangle(
                {
                    { scrollable_rect.position().x() + scrollable_rect.size().x() - 5,
                      scrollable_rect.position().y() + m_scroll.y() * scroll_area_size / content_size + 2 },
                    { 3, scroll_area_size / content_size * scroll_area_size - 4 },
                },
                scrollbar
            );
        }
    }
}

void ScrollableWidget::scroll_to_bottom() {
    if (raw_size().x() != 0) {
        auto bottom_content = content_size().x() - scroll_area_size().x();
        if (bottom_content > 0)
            m_scroll.set_x(bottom_content);
    }
    if (raw_size().y() != 0) {
        auto bottom_content = content_size().y() - scroll_area_size().y();
        if (bottom_content > 0)
            m_scroll.set_y(bottom_content);
    }
}

Util::Size2i ScrollableWidget::scroll_area_size() const {
    auto scrollable_rect = this->scrollable_rect();
    auto size = scrollable_rect.size();

    // We need to make place for scrollbars if content overflows
    auto content_size = this->content_size();
    if (content_size.y() > size.y())
        size.set_x(size.x() - 5);
    if (content_size.x() > size.x())
        size.set_y(size.y() - 5);
    return size;
}

void ScrollableWidget::relayout() {
    clamp_scrollbars();
}

}
