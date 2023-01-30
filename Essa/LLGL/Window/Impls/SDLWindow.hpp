#pragma once

#include "WindowImpl.hpp"

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <string>

namespace llgl {

class SDLWindowImpl : public WindowImpl {
public:
    ~SDLWindowImpl();
    virtual void create(Util::Vector2i size, Util::UString const& title, WindowSettings const&) override;
    virtual void close() override;
    virtual void set_title(Util::UString const&) override;
    virtual void set_size(Util::Vector2i) override;
    virtual void set_position(Util::Vector2i) override;
    virtual void display() override;
    virtual std::optional<Event> poll_event() override;
    virtual void set_mouse_position(Util::Vector2i) override;
    virtual bool is_focused() const override;
    virtual void set_active() override;
    virtual void maximize() override;
    virtual Util::Vector2i screen_size() override;
    virtual Util::Recti system_rect() override;

private:
    SDL_Window* m_window {};
    bool m_focused = false;
};

}
