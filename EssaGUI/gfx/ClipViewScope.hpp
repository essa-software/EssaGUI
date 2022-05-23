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

    ClipViewScope(GUI::SFMLWindow& target, sf::FloatRect rect, Mode);
    ~ClipViewScope();

private:
    sf::View create_clip_view(sf::FloatRect const&) const;

    GUI::SFMLWindow& m_target;
    sf::View m_old_view;
};

}
