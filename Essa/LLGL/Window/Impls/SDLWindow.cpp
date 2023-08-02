#include "SDLWindow.hpp"

#include "SDLHelpers.hpp"

#undef KeyPress   // Thanks C++
#undef KeyRelease // Thanks C++

#include <Essa/LLGL/Window/Event.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/UString.hpp>
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

void Window::create_impl(Util::Size2u size, Util::UString const& title, WindowSettings const& settings) {
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

#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
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
#endif

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
        }
#else
        fmt::print("SDLWindow: Transparent windows not supported for video driver {}\n", SDL_GetVideoDriver(0));
#endif
    }
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

    m_data = std::make_unique<Detail::SDLWindowData>();

    if (has_flag(settings.flags, WindowFlags::Shaped)) {
        m_data->window = SDL_CreateShapedWindow(
            (char*)title.encode().c_str(), 0, 0, size.x(), size.y(), SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | sdl_flags
        );
    }
    else {
        m_data->window
            = SDL_CreateWindow((char*)title.encode().c_str(), 0, 0, size.x(), size.y(), SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | sdl_flags);
    }
    if (!s_context)
        s_context = SDL_GL_CreateContext(m_data->window);
    if (!s_context) {
        fmt::print("Failed to create OpenGL context: {}\n", SDL_GetError());
    }
    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    std::cout << "SDLWindow: Created OpenGL context version " << major << "." << minor << std::endl;
    std::cout << "GL_VERSION=" << glGetString(GL_VERSION) << std::endl;
    set_active();
}

void Window::destroy() {
    if (is_closed()) {
        return;
    }
    SDL_DestroyWindow(m_data->window);
    m_data.reset();
    // SDL_GL_DeleteContext(s_context);
}

void Window::set_title(Util::UString const& title) {
    assert(!is_closed());
    SDL_SetWindowTitle(m_data->window, (char*)title.encode().c_str());
}

Util::UString Window::title() const { return Util::UString { SDL_GetWindowTitle(m_data->window) }; }

void Window::set_size_impl(Util::Size2u size) {
    assert(!is_closed());
    SDL_SetWindowSize(m_data->window, size.x(), size.y());
}

void Window::set_position(Util::Point2i position) {
    assert(!is_closed());
    SDL_SetWindowPosition(m_data->window, position.x(), position.y());
}

void Window::display() {
    assert(!is_closed());
    SDL_GL_SwapWindow(m_data->window);
}

static std::map<uint32_t, std::queue<SDL_Event>> s_event_queues;

std::optional<Event> Window::poll_event_impl() {
    assert(!is_closed());
    while (true) {
        std::optional<SDL_Event> sdl_event = [this]() -> std::optional<SDL_Event> {
            auto& queue = s_event_queues[SDL_GetWindowID(m_data->window)];
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

                if (sdl_event2.window.windowID != SDL_GetWindowID(m_data->window)) {
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
                m_data->focused = true;
                return Event::WindowFocusGained {};
            case SDL_WINDOWEVENT_FOCUS_LOST:
                m_data->focused = false;
                return Event::WindowFocusLost {};
            case SDL_WINDOWEVENT_CLOSE: {
                return Event::WindowClose {};
            } break;
            case SDL_WINDOWEVENT_ENTER: {
                return Event::MouseEnter {};
            } break;
            case SDL_WINDOWEVENT_LEAVE: {
                return Event::MouseLeave {};
            } break;
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
            return Event::KeyPress { static_cast<KeyCode>(sdl_event->key.keysym.sym), modifiers };
        }
        else if (sdl_event->type == SDL_KEYUP) {
            Event::Key::KeyModifiers modifiers;
            modifiers.ctrl = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_CTRL;
            modifiers.alt = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_ALT;
            modifiers.shift = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_SHIFT;
            modifiers.meta = sdl_event->key.keysym.mod & SDL_Keymod::KMOD_GUI;
            return Event::KeyRelease { static_cast<KeyCode>(sdl_event->key.keysym.sym), modifiers };
        }
        else if (sdl_event->type == SDL_MOUSEMOTION) {
            if (sdl_event->motion.xrel == 0 && sdl_event->motion.yrel == 0)
                continue;
            return Event::MouseMove { { sdl_event->motion.x, sdl_event->motion.y }, { sdl_event->motion.xrel, sdl_event->motion.yrel } };
        }
        else if (sdl_event->type == SDL_MOUSEBUTTONDOWN) {
            return Event::MouseButtonPress { { sdl_event->button.x, sdl_event->button.y },
                                             static_cast<MouseButton>(sdl_event->button.button) };
        }
        else if (sdl_event->type == SDL_MOUSEBUTTONUP) {
            return Event::MouseButtonRelease { { sdl_event->button.x, sdl_event->button.y },
                                               static_cast<MouseButton>(sdl_event->button.button) };
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
    }
}

void Window::set_mouse_position(Util::Point2i pos) {
    assert(!is_closed());
    SDL_WarpMouseInWindow(m_data->window, pos.x(), pos.y());
}

bool Window::is_focused() const {
    assert(!is_closed());
    return m_data->focused;
}

void Window::set_active() const {
    assert(!is_closed());
    SDL_GL_MakeCurrent(m_data->window, s_context);
}

void Window::maximize() {
    assert(!is_closed());
    SDL_MaximizeWindow(m_data->window);
    m_size = system_rect().size().cast<unsigned>();
}

Util::Size2u Window::screen_size() const {
    assert(!is_closed());
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(m_data->window), &mode);
    return { mode.w, mode.h };
}

Util::Recti Window::system_rect() const {
    assert(!is_closed());
    Util::Recti rect;
    SDL_GetWindowPosition(m_data->window, &rect.left, &rect.top);
    SDL_GetWindowSize(m_data->window, &rect.width, &rect.height);
    return rect;
}

Util::Point2i Window::position() const {
    int x;
    int y;
    SDL_GetWindowPosition(m_data->window, &x, &y);
    return { x, y };
}

}
