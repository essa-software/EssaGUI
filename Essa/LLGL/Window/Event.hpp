#pragma once

#include <Essa/LLGL/Window/Keyboard.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/CoordinateSystem.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>
#include <type_traits>
#include <variant>

namespace llgl {

struct Event;

enum class EventTargetType {
    KeyboardFocused, // Only keyboard-focused widgets are notified
    MouseFocused,    // Only mouse-focused (mouse hovered) widgets are notified. They have mouse position.
    Specific,        // Only specific widgets are notified (e.g focus events) (is affected = false for every widget)
    Global,          // All widgets are notified (e.g window resize) (is affected = true for every widget)
};

namespace EventTypes {

class Base {
public:
    static EventTargetType target_type() { return EventTargetType::Specific; }
};

class WindowResizeEvent : public Base {
public:
    explicit WindowResizeEvent(Util::Size2u new_size)
        : m_new_size(new_size) { }

    Util::Size2u new_size() const { return m_new_size; }
    static EventTargetType target_type() { return EventTargetType::Global; }

private:
    Util::Size2u m_new_size;
};

class KeyEvent : public Base {
public:
    struct KeyModifiers {
        bool ctrl;
        bool alt;
        bool shift;
        bool meta;
    };

    explicit KeyEvent(llgl::KeyCode key_code, KeyModifiers modifiers)
        : m_code(key_code)
        , m_modifiers(modifiers) { }

    auto code() const { return m_code; }
    auto modifiers() const { return m_modifiers; }
    static EventTargetType target_type() { return EventTargetType::KeyboardFocused; }

private:
    llgl::KeyCode m_code;
    KeyModifiers m_modifiers;
};

class KeyPressEvent : public KeyEvent {
public:
    explicit KeyPressEvent(llgl::KeyCode key_code, KeyModifiers modifiers)
        : KeyEvent(key_code, modifiers) { }
};

class KeyReleaseEvent : public KeyEvent {
public:
    explicit KeyReleaseEvent(llgl::KeyCode key_code, KeyModifiers modifiers)
        : KeyEvent(key_code, modifiers) { }
};

class MouseEvent : public Base {
public:
    explicit MouseEvent(Util::Point2i position)
        : m_local_position(position) { }

    // Position relative to thing that is currently handling the event
    // (Widget/WTR). The parent is responsible for transforming event using
    // Event::transformed() function.
    Util::Point2i local_position() const { return m_local_position; }
    static EventTargetType target_type() { return EventTargetType::MouseFocused; }

private:
    friend struct ::llgl::Event;

    void relativize(Util::Vector2i offset) { m_local_position -= offset; }

    Util::Point2i m_local_position;
};

class MouseMoveEvent : public MouseEvent {
public:
    explicit MouseMoveEvent(Util::Point2i position, Util::Vector2i delta)
        : MouseEvent(position)
        , m_delta(delta) { }

    Util::Vector2i delta() const { return m_delta; }

private:
    Util::Vector2i m_delta;
};

class MouseButtonEvent : public MouseEvent {
public:
    explicit MouseButtonEvent(Util::Point2i position, llgl::MouseButton button)
        : MouseEvent(position)
        , m_button(button) { }

    auto button() const { return m_button; }

private:
    llgl::MouseButton m_button;
};

class MouseButtonPressEvent : public MouseButtonEvent {
public:
    explicit MouseButtonPressEvent(Util::Point2i position, llgl::MouseButton button)
        : MouseButtonEvent(position, button) { }
};

class MouseButtonReleaseEvent : public MouseButtonEvent {
public:
    explicit MouseButtonReleaseEvent(Util::Point2i position, llgl::MouseButton button)
        : MouseButtonEvent(position, button) { }
};

class MouseScrollEvent : public MouseEvent {
public:
    explicit MouseScrollEvent(Util::Point2i position, float delta)
        : MouseEvent(position)
        , m_delta(delta) { }

    float delta() const { return m_delta; }

private:
    float m_delta;
};

class TextInputEvent : public Base {
public:
    explicit TextInputEvent(Util::UString text)
        : m_text(std::move(text)) { }

    Util::UString const& text() const { return m_text; }
    static EventTargetType target_type() { return EventTargetType::KeyboardFocused; }

private:
    Util::UString m_text;
};

using Variant = std::variant<
    EventTypes::WindowResizeEvent, EventTypes::KeyPressEvent, EventTypes::KeyReleaseEvent, EventTypes::MouseMoveEvent,
    EventTypes::MouseButtonPressEvent, EventTypes::MouseButtonReleaseEvent, EventTypes::MouseScrollEvent, EventTypes::TextInputEvent>;

}

struct Event : public EventTypes::Variant {
    using Variant = EventTypes::Variant;

    template<class T>
    Event(T&& t)
        requires(std::is_constructible_v<Variant, T>)
        : Variant(std::forward<T>(t)) { }

    using WindowResize = EventTypes::WindowResizeEvent;
    using Key = EventTypes::KeyEvent;
    using KeyPress = EventTypes::KeyPressEvent;
    using KeyRelease = EventTypes::KeyReleaseEvent;
    using Mouse = EventTypes::MouseEvent;
    using MouseMove = EventTypes::MouseMoveEvent;
    using MouseButton = EventTypes::MouseButtonEvent;
    using MouseButtonPress = EventTypes::MouseButtonPressEvent;
    using MouseButtonRelease = EventTypes::MouseButtonReleaseEvent;
    using MouseScroll = EventTypes::MouseScrollEvent;
    using TextInput = EventTypes::TextInputEvent;

    template<class T> bool is() const { return std::holds_alternative<T>(*this); }

    template<class T> T const* get() const { return is<T>() ? &std::get<T>(*this) : nullptr; }

    template<class T> T* get() { return is<T>() ? &std::get<T>(*this) : nullptr; }

    template<class... Callbacks> auto visit(Callbacks&&... callbacks) {
        using OverloadedType = Util::Overloaded<Callbacks...>;
        return std::visit(OverloadedType { callbacks... }, *this);
    }

    template<class... Callbacks> auto visit(Callbacks&&... callbacks) const {
        using OverloadedType = Util::Overloaded<Callbacks...>;
        return std::visit(OverloadedType { callbacks... }, *this);
    }

    EventTargetType target_type() const {
        return visit([](auto const& event) -> EventTargetType {
            using EventType = std::remove_cvref_t<decltype(event)>;
            return EventType::target_type();
        });
    }

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
    };

    bool is_mouse_related() const { return target_type() == EventTargetType::MouseFocused; }

    Util::Point2i local_mouse_position() const {
        assert(is_mouse_related());
        return visit([&](auto const& event) -> Util::Point2i {
            using EventType = std::remove_cvref_t<decltype(event)>;
            if constexpr (std::is_base_of_v<Event::Mouse, EventType>) {
                return event.local_position();
            }
            ESSA_UNREACHABLE;
        });
    };
};

}
