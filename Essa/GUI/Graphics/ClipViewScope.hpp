#pragma once

#include <Essa/GUI/Graphics/Painter.hpp>

namespace Gfx {

class ClipViewScope {
public:
    enum class Mode {
        Override, // current = new
        Intersect // current = old ∩ new
    };

    ClipViewScope(Gfx::Painter& target, Util::Vector2u host_window_size, Util::Recti rect, Mode);
    ~ClipViewScope();

private:
    static llgl::Projection create_clip_view(Util::Recti const&, Util::Vector2f offset_position, Util::Vector2u host_window_size);

    Gfx::Painter& m_target;
    llgl::Projection m_old_projection;
};

}
