#pragma once

#include <Essa/LLGL/Window/Event.hpp>

namespace GUI {

using EventVariant = std::variant<
    llgl::EventTypes::KeyPressEvent, llgl::EventTypes::KeyReleaseEvent, llgl::EventTypes::MouseEnterEvent,
    llgl::EventTypes::MouseLeaveEvent, llgl::EventTypes::MouseMoveEvent, llgl::EventTypes::MouseButtonPressEvent,
    llgl::EventTypes::MouseButtonReleaseEvent, llgl::EventTypes::MouseScrollEvent, llgl::EventTypes::TextInputEvent>;

class Event : public llgl::EventBase<EventVariant> {
public:
    using Key = llgl::EventTypes::KeyEvent;
    using KeyPress = llgl::EventTypes::KeyPressEvent;
    using KeyRelease = llgl::EventTypes::KeyReleaseEvent;
    using MouseEnter = llgl::EventTypes::MouseEnterEvent;
    using MouseLeave = llgl::EventTypes::MouseLeaveEvent;
    using Mouse = llgl::EventTypes::MouseEvent;
    using MouseMove = llgl::EventTypes::MouseMoveEvent;
    using MouseButton = llgl::EventTypes::MouseButtonEvent;
    using MouseButtonPress = llgl::EventTypes::MouseButtonPressEvent;
    using MouseButtonRelease = llgl::EventTypes::MouseButtonReleaseEvent;
    using MouseScroll = llgl::EventTypes::MouseScrollEvent;
    using TextInput = llgl::EventTypes::TextInputEvent;

    template<class T>
    Event(T&& t)
        : llgl::EventBase<EventVariant>(std::forward<T>(t)) { }

    Event relativized(Util::Vector2i offset) const {
        return visit([&](auto const& event) -> Event {
            using EventType = std::remove_cvref_t<decltype(event)>;
            if constexpr (std::is_base_of_v<Event::Mouse, EventType>) {
                auto new_event = event;
                new_event.relativize(offset);
                return new_event;
            }
            else {
                return event;
            }
        });
    }

    bool is_mouse_related() const { return target_type() == llgl::EventTargetType::MouseFocused; }

    Util::Point2i local_mouse_position() const {
        assert(is_mouse_related());
        return visit([&](auto const& event) -> Util::Point2i {
            using EventType = std::remove_cvref_t<decltype(event)>;
            if constexpr (std::is_base_of_v<Event::Mouse, EventType>) {
                return event.local_position();
            }
            ESSA_UNREACHABLE;
        });
    }
};

}
