#pragma once

#include <SFML/Graphics.hpp>

namespace Gfx {

class ClipViewScope {
public:
    enum class Mode {
        Override,   // current = new
        Intersect   // current = old ∩ new
    };

    ClipViewScope(sf::RenderTarget& target, sf::FloatRect rect, Mode);
    ~ClipViewScope();

private:
    sf::View create_clip_view(sf::FloatRect const&) const;

    sf::RenderTarget& m_target;
    sf::View m_old_view;
};

}
