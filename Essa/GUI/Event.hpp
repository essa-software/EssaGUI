#pragma once

#include <Essa/LLGL/Window/Event.hpp>
#include <fmt/format.h>

namespace GUI {

namespace EventTypes {

class MouseDoubleClick : public llgl::EventTypes::MouseEvent {
public:
    explicit MouseDoubleClick(Util::Point2i position)
        : MouseEvent(position) { }
};

class MouseEnterEvent : public llgl::EventTypes::Base {
public:
    static llgl::EventTargetType target_type() { return llgl::EventTargetType::Specific; }
};

class MouseLeaveEvent : public llgl::EventTypes::Base {
public:
    static llgl::EventTargetType target_type() { return llgl::EventTargetType::Specific; }
};

using Variant = std::variant<
    llgl::EventTypes::KeyPressEvent, llgl::EventTypes::KeyReleaseEvent, MouseEnterEvent, MouseLeaveEvent, llgl::EventTypes::MouseMoveEvent,
    llgl::EventTypes::MouseButtonPressEvent, llgl::EventTypes::MouseButtonReleaseEvent, MouseDoubleClick,
    llgl::EventTypes::MouseScrollEvent, llgl::EventTypes::TextInputEvent>;

}

class Event : public llgl::EventBase<EventTypes::Variant> {
public:
    using Key = llgl::EventTypes::KeyEvent;
    using KeyPress = llgl::EventTypes::KeyPressEvent;
    using KeyRelease = llgl::EventTypes::KeyReleaseEvent;

    using Mouse = llgl::EventTypes::MouseEvent;
    using MouseMove = llgl::EventTypes::MouseMoveEvent;
    using MouseButton = llgl::EventTypes::MouseButtonEvent;
    using MouseButtonPress = llgl::EventTypes::MouseButtonPressEvent;
    using MouseButtonRelease = llgl::EventTypes::MouseButtonReleaseEvent;
    using MouseScroll = llgl::EventTypes::MouseScrollEvent;
    using TextInput = llgl::EventTypes::TextInputEvent;

    using MouseDoubleClick = EventTypes::MouseDoubleClick;
    using MouseEnter = EventTypes::MouseEnterEvent;
    using MouseLeave = EventTypes::MouseLeaveEvent;

    template<class T>
    Event(T&& t)
        : llgl::EventBase<EventTypes::Variant>(std::forward<T>(t)) { }

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

template<> class fmt::formatter<GUI::Event> : public fmt::formatter<std::string_view> {
public:
    template<typename FormatContext> constexpr auto format(GUI::Event const& event, FormatContext& ctx) const {
        event.visit([&](auto const& evt) { fmt::format_to(ctx.out(), "{}", evt); });
        return ctx.out();
    }
};
template<> class fmt::formatter<GUI::EventTypes::MouseDoubleClick> : public fmt::formatter<std::string_view> {
public:
    template<typename FC> constexpr auto format(GUI::EventTypes::MouseDoubleClick const& event, FC& ctx) const {
        fmt::format_to(ctx.out(), "MouseDoubleClick(pos={})", event.local_position());
        return ctx.out();
    }
};
template<> class fmt::formatter<GUI::EventTypes::MouseEnterEvent> : public fmt::formatter<std::string_view> {
public:
    template<typename FC> constexpr auto format(GUI::EventTypes::MouseEnterEvent const&, FC& ctx) const {
        fmt::format_to(ctx.out(), "MouseEnterEvent");
        return ctx.out();
    }
};
template<> class fmt::formatter<GUI::EventTypes::MouseLeaveEvent> : public fmt::formatter<std::string_view> {
public:
    template<typename FC> constexpr auto format(GUI::EventTypes::MouseLeaveEvent const&, FC& ctx) const {
        fmt::format_to(ctx.out(), "MouseLeaveEvent");
        return ctx.out();
    }
};
