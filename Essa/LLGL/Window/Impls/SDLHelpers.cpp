#include "SDLHelpers.hpp"

#include <EssaUtil/ScopeGuard.hpp>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_video.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <cassert>
#include <fmt/core.h>
#include <fmt/format.h>

namespace llgl {

#define DEFER(a, ...) Util::ScopeGuard guard##a([&]() { __VA_ARGS__; })

int SDLHelpers::get_sdl_gl_attribute(SDL_GLattr id) {
    int value;
    assert(SDL_GL_GetAttribute(id, &value) >= 0);
    return value;
}

#ifdef SDL_VIDEO_DRIVER_X11
template<class T>
class XDeallocator {
public:
    void operator()(T* ptr) {
        XFree(ptr);
    }
};
template<class T>
using XUniquePtr = std::unique_ptr<T, XDeallocator<T>>;

std::optional<VisualID> SDLHelpers::X11::get_transparent_visual_id() {
    auto x_display = XOpenDisplay(NULL);
    if (!x_display) {
        fmt::print("Couldn't connect to X server\n");
        return {};
    }
    auto x_screen = DefaultScreen(x_display);

    auto profile_mask = get_sdl_gl_attribute(SDL_GL_CONTEXT_PROFILE_MASK);
    static int attributes[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, get_sdl_gl_attribute(SDL_GL_DOUBLEBUFFER),
        GLX_CONTEXT_MAJOR_VERSION_ARB, get_sdl_gl_attribute(SDL_GL_CONTEXT_MAJOR_VERSION),
        GLX_CONTEXT_MINOR_VERSION_ARB, get_sdl_gl_attribute(SDL_GL_CONTEXT_MINOR_VERSION),
        GL_CONTEXT_CORE_PROFILE_BIT, profile_mask & SDL_GL_CONTEXT_PROFILE_CORE,
        GL_CONTEXT_COMPATIBILITY_PROFILE_BIT, profile_mask & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
        GLX_RED_SIZE, get_sdl_gl_attribute(SDL_GL_RED_SIZE),
        GLX_GREEN_SIZE, get_sdl_gl_attribute(SDL_GL_GREEN_SIZE),
        GLX_BLUE_SIZE, get_sdl_gl_attribute(SDL_GL_BLUE_SIZE),
        GLX_ALPHA_SIZE, get_sdl_gl_attribute(SDL_GL_ALPHA_SIZE),
        GLX_DEPTH_SIZE, get_sdl_gl_attribute(SDL_GL_DEPTH_SIZE),
        None
    };

    int numfbconfigs = 0;
    auto fbconfigs = glXChooseFBConfig(x_display, x_screen, attributes, &numfbconfigs);
    XUniquePtr<XVisualInfo> visual = nullptr;

    for (int i = 0; i < numfbconfigs; i++) {
        visual = XUniquePtr<XVisualInfo> { glXGetVisualFromFBConfig(x_display, fbconfigs[i]) };
        if (!visual) {
            continue;
        }

        auto pict_format = XRenderFindVisualFormat(x_display, visual->visual);
        if (!pict_format) {
            continue;
        }

        if (pict_format->direct.alphaMask > 0) {
            break;
        }
    }

    if (!visual) {
        fmt::print("No matching visual found\n");
        return {};
    }

    int visual_id = visual->visualid;
    return visual_id;
}
#endif

uint32_t SDLHelpers::llgl_window_flags_to_sdl(WindowFlags flags) {
    uint32_t sdl_flags = 0;
    if (has_flag(flags, WindowFlags::Fullscreen)) {
        sdl_flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (has_flag(flags, WindowFlags::Borderless)) {
        sdl_flags |= SDL_WINDOW_BORDERLESS;
    }
    if (has_flag(flags, WindowFlags::Resizable)) {
        sdl_flags |= SDL_WINDOW_RESIZABLE;
    }
    if (has_flag(flags, WindowFlags::Minimized)) {
        sdl_flags |= SDL_WINDOW_MINIMIZED;
    }
    if (has_flag(flags, WindowFlags::Maximized)) {
        sdl_flags |= SDL_WINDOW_MAXIMIZED;
    }
    return sdl_flags;
}

}
