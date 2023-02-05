#include "SDLWindow.hpp"

#include "../Event.hpp"
#include "SDLHelpers.hpp"
#include <Essa/LLGL/Window/Mouse.hpp>
#include <EssaUtil/UString.hpp>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <queue>

namespace llgl {

static SDL_GLContext s_context = nullptr;

SDLWindowImpl::~SDLWindowImpl() {
    close();
}

void SDLWindowImpl::create(Util::Vector2i size, Util::UString const& title, WindowSettings const& settings) {
    static bool initialized = false;
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    if (!initialized && SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDLWindow: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    if (SDL_GL_LoadLibrary(nullptr) < 0) {
        std::cout << "SDLWindow: Failed to load GL library" << std::endl;
        exit(1);
    }

    initialized = true;

    // Note: Remember to add these attributes to GLX in transparent window implementation
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.context_settings.major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.context_settings.minor_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    uint32_t sdl_flags = SDLHelpers::llgl_window_flags_to_sdl(settings.flags);
    if (has_flag(settings.flags, WindowFlags::TransparentBackground)) {
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
#ifdef SDL_VIDEO_DRIVER_X11
        auto forced_visual_id = SDLHelpers::X11::get_transparent_visual_id();
        if (!forced_visual_id) {
            fmt::print("SDLWindow: Transparent windows not supported\n");
        }
        else {
            SDL_SetHint(SDL_HINT_VIDEO_X11_WINDOW_VISUALID, std::to_string(*forced_visual_id).c_str());
            SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
        }
#else
        fmt::print("SDLWindow: Transparent windows not supported for video driver {}\n", SDL_GetVideoDriver(0));
#endif
    }
    m_window = SDL_CreateWindow((char*)title.encode().c_str(), 0, 0, size.x(), size.y(), SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | sdl_flags);
    if (!s_context)
        s_context = SDL_GL_CreateContext(m_window);
    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    std::cout << "SDLWindow: Created OpenGL context version " << major << "." << minor << std::endl;
}

void SDLWindowImpl::close() {
    if (!m_window)
        return;
    SDL_DestroyWindow(m_window);
    // SDL_GL_DeleteContext(s_context);
}

void SDLWindowImpl::set_title(Util::UString const& title) {
    if (!m_window)
        return;
    SDL_SetWindowTitle(m_window, (char*)title.encode().c_str());
}

void SDLWindowImpl::set_size(Util::Vector2i size) {
    if (!m_window)
        return;
    SDL_SetWindowSize(m_window, size.x(), size.y());
}

void SDLWindowImpl::set_position(Util::Vector2i position) {
    SDL_SetWindowPosition(m_window, position.x(), position.y());
}

void SDLWindowImpl::display() {
    SDL_GL_SwapWindow(m_window);
}

static std::map<uint32_t, std::queue<SDL_Event>> s_event_queues;

std::optional<Event> SDLWindowImpl::poll_event() {
    while (true) {
        std::optional<SDL_Event> sdl_event = [this]() -> std::optional<SDL_Event> {
            auto& queue = s_event_queues[SDL_GetWindowID(m_window)];
            if (!queue.empty()) {
                auto queued_event = queue.front();
                queue.pop();
                return queued_event;
            }
            else {
                SDL_Event sdl_event2;
                auto is_event = SDL_PollEvent(&sdl_event2);
                if (!is_event)
                    return {};

                if (sdl_event2.window.windowID != SDL_GetWindowID(m_window)) {
                    s_event_queues[sdl_event2.window.windowID].push(sdl_event2);
                    return {};
                }
                return sdl_event2;
            }
        }();

        if (!sdl_event)
            return {};

        if (sdl_event->type == SDL_WINDOWEVENT) {
            bool should_continue = false;
            switch (sdl_event->window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                return Event::WindowResize({ sdl_event->window.data1, sdl_event->window.data2 });
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                m_focused = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                m_focused = false;
                break;
            case SDL_WINDOWEVENT_CLOSE: {
                // TODO: This is a hack to make Ctrl+C working. This should be
                // properly exposed to user.
                std::cout << "Exit requested" << std::endl;
                exit(0);
            }
            default:
                std::cout << "SDLWindow: Unhandled window event (type=" << (int)sdl_event->window.event << ")" << std::endl;
                should_continue = true;
            }
            if (should_continue) {
                continue;
            }
        }
        else if (sdl_event->type == SDL_KEYDOWN) {
            Event::Key::KeyModifiers modifiers;
            modifiers.ctrl = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_CTRL;
            modifiers.alt = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_ALT;
            modifiers.shift = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_SHIFT;
            modifiers.meta = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_GUI;
#undef KeyPress // Thanks C++
            return Event::KeyPress { static_cast<KeyCode>(sdl_event->key.keysym.sym), modifiers };
        }
        else if (sdl_event->type == SDL_KEYUP) {
            Event::Key::KeyModifiers modifiers;
            modifiers.ctrl = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_CTRL;
            modifiers.alt = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_ALT;
            modifiers.shift = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_SHIFT;
            modifiers.meta = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_GUI;
#undef KeyRelease // Thanks C++
            return Event::KeyRelease { static_cast<KeyCode>(sdl_event->key.keysym.sym), modifiers };
        }
        else if (sdl_event->type == SDL_MOUSEMOTION) {
            if (sdl_event->motion.xrel == 0 && sdl_event->motion.yrel == 0)
                continue;
            return Event::MouseMove { { sdl_event->motion.x, sdl_event->motion.y }, { sdl_event->motion.xrel, sdl_event->motion.yrel } };
        }
        else if (sdl_event->type == SDL_MOUSEBUTTONDOWN) {
            return Event::MouseButtonPress { { sdl_event->button.x, sdl_event->button.y }, static_cast<MouseButton>(sdl_event->button.button) };
        }
        else if (sdl_event->type == SDL_MOUSEBUTTONUP) {
            return Event::MouseButtonRelease { { sdl_event->button.x, sdl_event->button.y }, static_cast<MouseButton>(sdl_event->button.button) };
        }
        else if (sdl_event->type == SDL_MOUSEWHEEL) {
            auto mouse = mouse_position();
            return Event::MouseScroll { { mouse.x(), mouse.y() }, sdl_event->wheel.preciseY };
        }
        else if (sdl_event->type == SDL_TEXTINPUT) {
            Util::UString input_string { { sdl_event->text.text, strlen(sdl_event->text.text) } };
            if (input_string.is_empty())
                continue;
            return Event::TextInput { input_string };
        }
        // TODO
        std::cout << "SDLWindow: Unhandled event (type=" << sdl_event->type << ")" << std::endl;
        continue;
    }
}

void SDLWindowImpl::set_mouse_position(Util::Vector2i pos) {
    SDL_WarpMouseInWindow(m_window, pos.x(), pos.y());
}

bool SDLWindowImpl::is_focused() const {
    return m_focused;
}

void SDLWindowImpl::set_active() {
    SDL_GL_MakeCurrent(m_window, s_context);
}

void SDLWindowImpl::maximize() {
    SDL_MaximizeWindow(m_window);
}

Util::Vector2i SDLWindowImpl::screen_size() {
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(m_window), &mode);
    return { mode.w, mode.h };
}

Util::Recti SDLWindowImpl::system_rect() {
    Util::Recti rect;
    SDL_GetWindowPosition(m_window, &rect.left, &rect.top);
    SDL_GetWindowSize(m_window, &rect.width, &rect.height);
    return rect;
}

}
