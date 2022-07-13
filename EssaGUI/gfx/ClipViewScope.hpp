#pragma once

#include "EssaGUI/gfx/SFMLWindow.hpp"
#include <SFML/Graphics.hpp>

namespace Gfx {

class ClipViewScope {
public:
    enum class Mode {
        Override,   // current = new
        Intersect   // current = old ∩ new
    };

    ClipViewScope(GUI::SFMLWindow& target, Util::Rectf rect, Mode);
    ~ClipViewScope();

private:
    sf::View create_clip_view(Util::Rectf const&) const;

    GUI::SFMLWindow& m_target;
    sf::View m_old_view;
};

}
