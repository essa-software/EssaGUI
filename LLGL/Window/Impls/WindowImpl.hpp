#pragma once

#include "../ContextSettings.hpp"

#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>
#include <string>

namespace llgl {

class Event;

class WindowImpl {
public:
    virtual ~WindowImpl() = default;
    virtual void create(Util::Vector2i size, Util::UString const& title, ContextSettings const&) = 0;
    virtual void close() = 0;
    virtual void set_title(Util::UString const&) = 0;
    virtual void set_size(Util::Vector2i) = 0;
    virtual void display() = 0;
    virtual bool poll_event(Event&) = 0;
    virtual void set_mouse_position(Util::Vector2i) = 0;
    virtual bool is_focused() const = 0;
    virtual void set_active() = 0;
    virtual void maximize() = 0;
};

}
