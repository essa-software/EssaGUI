#include "ScrollableContainer.hpp"

namespace GUI {

ScrollableContainer::ScrollableContainer() {
    on_scroll = [&]() {
        m_widget->set_raw_position(scroll_offset().to_point());

        // FIXME: The relayout is needed because widgets use wtr-relative (i.e
        // global) position for rendering. If they used container-relative
        // position, we could avoid relayouting ScrollableContainer contents on
        // every scroll!
        m_widget->do_relayout();
    };
}

Util::Cs::Size2i ScrollableContainer::content_size() const {
    return m_widget->total_size();
}

Widget::EventHandlerResult ScrollableContainer::do_handle_event(
    Event const& event) {
    Widget::do_handle_event(event);
    if (ScrollableWidget::handle_event(event) == EventHandlerResult::Accepted) {
        return EventHandlerResult::Accepted;
    }
    return m_widget->do_handle_event(event);
}

void ScrollableContainer::draw(Gfx::Painter& painter) const {
    m_widget->do_draw(painter);
    ScrollableWidget::draw_scrollbar(painter);
}

void ScrollableContainer::do_relayout() {
    Widget::do_relayout();

    // 1. Do first relayout with the full available size.
    m_widget->set_raw_size(raw_size());
    m_widget->do_relayout();

    // 2. If the widget doesn't fit, relayout once more (because we will add
    // scrollbars, which will change available size).
    auto actually_needed_size = raw_size();
    if (m_widget->total_size().x() > raw_size().x()) {
        actually_needed_size.set_y(actually_needed_size.x() - 5);
    }
    if (m_widget->total_size().y() > raw_size().y()) {
        actually_needed_size.set_x(actually_needed_size.x() - 5);
    }
    fmt::print("{} != {} ?\n", actually_needed_size, raw_size());
    if (actually_needed_size != raw_size()) {
        m_widget->set_raw_size(actually_needed_size);
        m_widget->do_relayout();
    }
}

void ScrollableContainer::dump(unsigned depth) {
    Widget::dump(depth);
    for (unsigned i = 0; i < depth; i++)
        std::cout << "-   ";

    if (!is_visible())
        return;
    ++depth;
    m_widget->dump(depth);
}

// FIXME: Implement EML loader.

}
