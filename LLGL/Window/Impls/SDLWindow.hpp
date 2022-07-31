#pragma once

#include "WindowImpl.hpp"

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <string>

namespace llgl {

class SDLWindowImpl : public WindowImpl {
public:
    ~SDLWindowImpl();
    virtual void create(Util::Vector2i size, Util::UString const& title, ContextSettings const&) override;
    virtual void close() override;
    virtual void set_title(Util::UString const&) override;
    virtual void set_size(Util::Vector2i) override;
    virtual void display() override;
    virtual bool poll_event(Event&) override;
    virtual void set_mouse_position(Util::Vector2i) override;
    virtual bool is_focused() const override;

private:
    SDL_Window* m_window {};
    SDL_GLContext m_context;
    bool m_focused = false;
};

}
