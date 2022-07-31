#include "Utils.hpp"

#include "Error.hpp"
#include <GL/gl.h>
#include <set>

namespace llgl::opengl {

void set_clear_color(Util::Color color)
{
    glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}

void clear(ClearMask mask)
{
    glClear(static_cast<GLbitfield>(mask));
}

static ClearMask s_buffers_to_clear = ClearMask::Color;

void clear_enabled()
{
    clear(s_buffers_to_clear);
}

void enable(Feature feature)
{
    if (feature == Feature::DepthTest)
        s_buffers_to_clear |= ClearMask::Depth;
    glEnable(static_cast<GLenum>(feature));
}

void disable(Feature feature)
{
    if (feature == Feature::DepthTest)
        s_buffers_to_clear &= ~ClearMask::Depth;
    glDisable(static_cast<GLenum>(feature));
}

void ensure_enabled(Feature feature)
{
    if (is_enabled(feature))
        return;
    enable(feature);
}

bool is_enabled(Feature feature)
{
    return glIsEnabled(static_cast<GLenum>(feature));
}

void set_scissor(Util::Recti rect)
{
    ensure_enabled(Feature::ScissorTest);
    glScissor(rect.left, rect.top, rect.width, rect.height);
}

}
