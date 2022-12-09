#pragma once

#include <EssaGUI/Graphics/Painter.hpp>

namespace Gfx {

class ClipViewScope {
public:
    enum class Mode {
        Override, // current = new
        Intersect // current = old ∩ new
    };

    ClipViewScope(Gfx::Painter& target, Util::Vector2u host_window_size, Util::Rectf rect, Mode);
    ~ClipViewScope();

private:
    llgl::Projection create_clip_view(Util::Rectf const&, Util::Vector2f offset_position, Util::Vector2u host_window_size) const;

    Gfx::Painter& m_target;
    llgl::Projection m_old_projection;
};

}
