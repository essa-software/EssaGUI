#pragma once

#include "../WindowSettings.hpp"
#include <Essa/LLGL/Window/Event.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>
#include <string>

namespace llgl {

class DeprecatedEvent;

class WindowImpl {
public:
    virtual ~WindowImpl() = default;
    virtual void create(Util::Vector2i size, Util::UString const& title, WindowSettings const&) = 0;
    virtual void close() = 0;
    virtual void set_title(Util::UString const&) = 0;
    virtual void set_size(Util::Vector2i) = 0;
    virtual void set_position(Util::Vector2i) = 0;
    virtual void display() = 0;
    virtual std::optional<Event> poll_event() = 0;
    virtual void set_mouse_position(Util::Vector2i) = 0;
    virtual bool is_focused() const = 0;
    virtual void set_active() = 0;
    virtual void maximize() = 0;
    virtual Util::Recti system_rect() = 0;
    virtual Util::Vector2i screen_size() = 0;
};

}
