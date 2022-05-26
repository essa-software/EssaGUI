#include "ScrollableWidget.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"

namespace GUI {

void ScrollableWidget::handle_event(Event& event) {
    switch (event.type()) {
    case sf::Event::MouseWheelScrolled: {
        if (!is_hover() || event.event().mouseWheelScroll.wheel != sf::Mouse::VerticalWheel)
            break;
        event.set_handled();
        if (content_height() < scroll_area_height())
            break;
        m_scroll -= event.event().mouseWheelScroll.delta * 60;
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

sf::Vector2f ScrollableWidget::scroll_offset() const {
    return { 0, -m_scroll };
}

void ScrollableWidget::set_scroll(float scroll) {
    m_scroll = scroll;
}

void ScrollableWidget::draw_scrollbar(GUI::SFMLWindow& window) const {
    // "Scrollbar"
    float scroll_area_size = this->scroll_area_height();
    float content_size = this->content_height();
    if (content_size > scroll_area_size) {
        RectangleDrawOptions scrollbar;
        scrollbar.fill_color = sf::Color { 200, 200, 200 };
        window.draw_rectangle({ { size().x - 5, m_scroll * scroll_area_size / content_size + 2 }, { 3, scroll_area_size / content_size * scroll_area_size - 4 } }, scrollbar);
    }
}

float ScrollableWidget::scroll_area_height() const {
    return size().y;
}

}
