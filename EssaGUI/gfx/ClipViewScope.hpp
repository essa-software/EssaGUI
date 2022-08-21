#pragma once

#include <EssaGUI/gfx/Window.hpp>

namespace Gfx {

class ClipViewScope {
public:
    enum class Mode {
        Override, // current = new
        Intersect // current = old ∩ new
    };

    ClipViewScope(GUI::Window& target, Util::Rectf rect, Mode);
    ~ClipViewScope();

private:
    llgl::Projection create_clip_view(Util::Rectf const&, Util::Vector2f offset_position) const;

    GUI::Window& m_target;
    llgl::Projection m_old_projection;
};

}
