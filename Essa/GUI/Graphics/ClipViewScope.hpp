#pragma once

#include <Essa/LLGL/OpenGL/Projection.hpp>

namespace Gfx {

class Painter;

// A RAII class that handles clipping of Painter's viewport.
// It maintains a stack of ClipViewScopes so that you don't
// need to know anything about calling environment (Widget/
// Overlay) to draw within it.
//
// Note that it doesn't actually set the viewport. If you use
// it with raw LLGL/OpenGL, wrap drawing calls in WorldDrawScope
// or set viewport manually:
//
// ```
// llgl::set_viewport(painter.builder().projection().viewport())
// ```
class ClipViewScope {
public:
    enum class Mode {
        Override,  // current = new
        Intersect, // current = old ∩ new
        NewStack,  // current = new and ignore parent CSV
    };

    // Rect is relative to the current projection viewport.
    ClipViewScope(Painter& target, Util::Recti rect, Mode);
    ~ClipViewScope();

private:
    static llgl::Projection create_clip_view(Util::Recti const&, Util::Vector2i offset_position, Util::Size2u framebuffer_size);

    Painter& m_target;
    llgl::Projection m_old_projection;
    Util::Vector2i m_offset;
    ClipViewScope* m_parent = nullptr;
};

}
