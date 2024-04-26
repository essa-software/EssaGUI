#pragma once

#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Rect.hpp>
#include <functional>

namespace GUI {

// A class that encapsulates button-like mouse behavior.
class ButtonBehavior {
public:
    using HitTester = std::function<bool(Util::Point2i)>;

    // Ensure that `rect` uses the same coordinate system as
    // incoming events!
    explicit ButtonBehavior(HitTester tester)
        : m_hit_tester(std::move(tester)) { }

    std::function<void()> on_click;

    Widget::EventHandlerResult on_mouse_move(Event::MouseMove const&);
    Widget::EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&);
    Widget::EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const&);

    bool is_hovered() const {
        return m_hovered;
    }

private:
    HitTester m_hit_tester;
    bool m_hovered = false;
    bool m_pressed_on = false;
};

}
