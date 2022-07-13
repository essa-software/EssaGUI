#include "ClipViewScope.hpp"

namespace Gfx {

ClipViewScope::ClipViewScope(GUI::SFMLWindow& target, Util::Rectf rect, Mode mode)
    : m_target(target)
    , m_old_view(target.view()) {

    auto old_viewport = m_old_view.getViewport();
    auto clip_rect = [&]() -> sf::FloatRect {
        auto rect_sf = sf::FloatRect { rect.left, rect.top, rect.width, rect.height };
        switch (mode) {
        case Mode::Override:
            return rect_sf;
        case Mode::Intersect: {
            // TODO: Port to Util when it gets intersects()
            sf::FloatRect old_clip_rect {
                old_viewport.left * target.getSize().x,
                old_viewport.top * target.getSize().y,
                old_viewport.width * target.getSize().x,
                old_viewport.height * target.getSize().y,
            };
            sf::FloatRect new_clip_rect;
            old_clip_rect.intersects(rect_sf, new_clip_rect);
            return new_clip_rect;
        }
        }
        __builtin_unreachable();
    }();
    auto clip_view = create_clip_view({ clip_rect.left, clip_rect.top, clip_rect.width, clip_rect.height });
    if (mode == Mode::Intersect) {
        auto x = rect.left;
        auto y = rect.top;
        auto vx = clip_view.getViewport().left;
        auto vy = clip_view.getViewport().top;
        Util::Vector2f offset_position { vx * target.getSize().x - x, vy * target.getSize().y - y };
        clip_view.move({ offset_position.x(), offset_position.y() });
    }
    m_target.set_view(clip_view);
}

ClipViewScope::~ClipViewScope() {
    m_target.set_view(m_old_view);
}

sf::View ClipViewScope::create_clip_view(Util::Rectf const& rect) const {

    sf::View clip_view { { 0, 0, rect.width, rect.height } };
    auto window_size = m_target.getSize();
    clip_view.setViewport(sf::FloatRect {
        rect.left / window_size.x, rect.top / window_size.y,
        rect.width / window_size.x, rect.height / window_size.y });
    return clip_view;
}

}
