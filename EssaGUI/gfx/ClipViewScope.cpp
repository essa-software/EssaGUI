#include "ClipViewScope.hpp"

namespace Gfx {

ClipViewScope::ClipViewScope(GUI::Window& target, Util::Rectf rect, Mode mode)
    : m_target(target)
    , m_old_view(target.view()) {

    auto old_viewport = m_old_view.viewport();
    auto clip_rect = [&]() {
        switch (mode) {
        case Mode::Override:
        case Mode::Intersect: {
            Util::Recti old_clip_rect {
                old_viewport.left,
                old_viewport.top,
                old_viewport.width,
                old_viewport.height,
            };
            return old_clip_rect.intersection(rect);
        }
        }
        __builtin_unreachable();
    }();
    auto clip_view = create_clip_view({ clip_rect.left, clip_rect.top, clip_rect.width, clip_rect.height });
    if (mode == Mode::Intersect) {
        auto x = rect.left;
        auto y = rect.top;
        auto vx = clip_view.viewport().left;
        auto vy = clip_view.viewport().top;
        Util::Vector2f offset_position { vx - x, vy - y };

        // TODO: Add some kind of move()
        auto ortho = clip_view.ortho_args();
        ortho.left += offset_position.x();
        ortho.right += offset_position.x();
        ortho.top += offset_position.y();
        ortho.bottom += offset_position.y();
        clip_view.set_ortho(ortho);
    }
    m_target.set_view(clip_view);
}

ClipViewScope::~ClipViewScope() {
    m_target.set_view(m_old_view);
}

llgl::View ClipViewScope::create_clip_view(Util::Rectf const& rect) const {

    llgl::View view;
    view.set_ortho({ { 0, 0, rect.width, rect.height } });
    view.set_viewport(Util::Recti { rect });
    return view;
}

}
