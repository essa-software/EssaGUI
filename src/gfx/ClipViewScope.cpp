#include "ClipViewScope.hpp"

namespace Gfx {

ClipViewScope::ClipViewScope(sf::RenderTarget& target, sf::FloatRect rect, Mode mode)
    : m_target(target)
    , m_old_view(target.getView()) {

    auto clip_rect = [&]() {
        switch (mode) {
        case Mode::Override:
            return rect;
        case Mode::Intersect: {
            auto old_viewport = m_old_view.getViewport();
            sf::FloatRect old_clip_rect {
                old_viewport.left * target.getSize().x,
                old_viewport.top * target.getSize().y,
                old_viewport.width * target.getSize().x,
                old_viewport.height * target.getSize().y,
            };
            sf::FloatRect new_clip_rect;
            old_clip_rect.intersects(rect, new_clip_rect);
            return new_clip_rect;
        }
        }
        __builtin_unreachable();
    }();
    m_target.setView(create_clip_view(clip_rect));
}

ClipViewScope::~ClipViewScope() {
    m_target.setView(m_old_view);
}

sf::View ClipViewScope::create_clip_view(sf::FloatRect const& rect) const {

    sf::View clip_view { { 0, 0, rect.width, rect.height } };
    auto window_size = m_target.getSize();
    clip_view.setViewport(sf::FloatRect {
        rect.left / window_size.x, rect.top / window_size.y,
        rect.width / window_size.x, rect.height / window_size.y });
    return clip_view;
}

}
