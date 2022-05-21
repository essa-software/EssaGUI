#include "WidgetTreeRoot.hpp"

#include "Application.hpp"
#include "Widget.hpp"

#include <cassert>
#include <iostream>

namespace GUI {

void WidgetTreeRoot::set_focused_widget(Widget* w) {
    m_focused_widget = w;
}

void WidgetTreeRoot::draw() {
    sf::View gui_view { sf::FloatRect(position(), size()) };
    m_window.setView(gui_view);

    if (m_needs_relayout) {
        // std::cout << m_id << "\n"
        m_main_widget->set_size({ { size().x, Length::Unit::Px }, { size().y, Length::Unit::Px } });
        m_main_widget->set_raw_size(size());
        m_main_widget->do_relayout();
        // m_main_widget->dump(0);
        m_needs_relayout = false;
    }
    m_main_widget->do_draw(m_window);
}

void WidgetTreeRoot::handle_event(sf::Event event) {
    Event gui_event(event);
    m_main_widget->do_handle_event(gui_event);
}

void WidgetTreeRoot::tick() {
    // Handle events only for the current WTR...
    // (so that e.g. the simulation doesn't run)
    handle_events();
    update();

    // HACK: Check for Closed events on App and break
    //       this loop so that we can quit safely.
    if (!Application::the().is_running())
        quit();

    // ...but redraw the entire Application!
    window().clear();
    Application::the().draw();
    window().display();

    // Remove closed overlays on Application so
    // that closed windows actually are closed
    if (!is_running())
        return;
    Application::the().remove_closed_overlays();
}

sf::Event WidgetTreeRoot::transform_event(sf::Vector2f offset, sf::Event event) const {
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        event.mouseButton.x -= offset.x;
        event.mouseButton.y -= offset.y;
    }
    else if (event.type == sf::Event::MouseMoved) {
        event.mouseMove.x -= offset.x;
        event.mouseMove.y -= offset.y;
    }

    return event;
}

bool WidgetTreeRoot::pass_event_to_window_if_needed(WidgetTreeRoot& wtr, sf::Event event) {
    wtr.handle_event(transform_event(wtr.position(), event));
    bool scroll_outside_window = event.type == sf::Event::MouseWheelScrolled
        && !wtr.full_rect().contains({ static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y) });
    return !(event.type == sf::Event::Closed || event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased || scroll_outside_window);
}

}
