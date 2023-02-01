#pragma once

#include <Essa/LLGL/Window/WindowSettings.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>
#include <SDL2/SDL_video.h>

#ifdef SDL_VIDEO_DRIVER_X11
#    include <X11/Xlib.h>
#endif

namespace llgl::SDLHelpers {

#ifdef SDL_VIDEO_DRIVER_X11
namespace X11 {
std::optional<VisualID> get_transparent_visual_id();
}
#endif
int get_sdl_gl_attribute(SDL_GLattr id);
uint32_t llgl_window_flags_to_sdl(WindowFlags flags);

}
