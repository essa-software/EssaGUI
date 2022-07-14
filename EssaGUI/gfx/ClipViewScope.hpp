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
    llgl::View create_clip_view(Util::Rectf const&) const;

    GUI::Window& m_target;
    llgl::View m_old_view;
};

}
