#include "SDLWindow.hpp"

#include "../Event.hpp"
#include "LLGL/Window/Mouse.hpp"

#include <EssaUtil/UString.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
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

void SDLWindowImpl::create(Util::Vector2i size, Util::UString const& title, ContextSettings const& settings) {
    static bool initialized = false;
    if (!initialized && SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDLWindow: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    if (SDL_GL_LoadLibrary(nullptr) < 0) {
        std::cout << "SDLWindow: Failed to load GL library" << std::endl;
        exit(1);
    }
    initialized = true;

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minor_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    m_window = SDL_CreateWindow((char*)title.encode().c_str(), 0, 0, size.x(), size.y(), SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    std::cout << "SDLWindow: Created OpenGL context version " << major << "." << minor << std::endl;
    if (!s_context)
        s_context = SDL_GL_CreateContext(m_window);
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

void SDLWindowImpl::display() {
    SDL_GL_SwapWindow(m_window);
}

static std::map<uint32_t, std::queue<SDL_Event>> s_event_queues;

bool SDLWindowImpl::poll_event(Event& event) {
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
            return false;

        if (sdl_event->type == SDL_WINDOWEVENT) {
            switch (sdl_event->window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                event.type = Event::Type::Resize;
                event.resize.size = { sdl_event->window.data1, sdl_event->window.data2 };
                break;
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
                return false;
            }
            return true;
        }
        else if (sdl_event->type == SDL_KEYDOWN) {
            event.type = Event::Type::KeyPress;
            event.key.keycode = static_cast<KeyCode>(sdl_event->key.keysym.sym);
            event.key.shift = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_SHIFT;
            event.key.ctrl = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_CTRL;
            event.key.alt = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_ALT;
            event.key.meta = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_GUI;
            return true;
        }
        else if (sdl_event->type == SDL_KEYUP) {
            event.type = Event::Type::KeyRelease;
            event.key.keycode = static_cast<KeyCode>(sdl_event->key.keysym.sym);
            event.key.shift = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_SHIFT;
            event.key.ctrl = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_CTRL;
            event.key.alt = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_ALT;
            event.key.meta = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_GUI;
            return true;
        }
        else if (sdl_event->type == SDL_MOUSEMOTION) {
            if (sdl_event->motion.xrel == 0 && sdl_event->motion.yrel == 0)
                continue;
            event.type = Event::Type::MouseMove;
            event.mouse_move.position = { sdl_event->motion.x, sdl_event->motion.y };
            event.mouse_move.relative = { sdl_event->motion.xrel, sdl_event->motion.yrel };
            return true;
        }
        else if (sdl_event->type == SDL_MOUSEBUTTONDOWN) {
            event.type = Event::Type::MouseButtonPress;
            event.mouse_button.button = static_cast<MouseButton>(sdl_event->button.button);
            event.mouse_move.position = { sdl_event->button.x, sdl_event->button.y };
            return true;
        }
        else if (sdl_event->type == SDL_MOUSEBUTTONUP) {
            event.type = Event::Type::MouseButtonRelease;
            event.mouse_button.button = static_cast<MouseButton>(sdl_event->button.button);
            event.mouse_move.position = { sdl_event->button.x, sdl_event->button.y };
            return true;
        }
        else if (sdl_event->type == SDL_MOUSEWHEEL) {
            event.type = Event::Type::MouseScroll;
            auto mouse = mouse_position();
            event.mouse_scroll.position = { mouse.x(), mouse.y() };
            event.mouse_scroll.delta = sdl_event->wheel.preciseY;
            return true;
        }
        else if (sdl_event->type == SDL_TEXTINPUT) {
            event.type = Event::Type::TextInput;
            Util::UString input_string { { sdl_event->text.text, strlen(sdl_event->text.text) } };
            if (input_string.is_empty())
                continue;
            event.text_input.codepoint = input_string.at(0);
            return true;
        }
        // TODO
        std::cout << "SDLWindow: Unhandled event (type=" << sdl_event->type << ")" << std::endl;
        return false;
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

}
