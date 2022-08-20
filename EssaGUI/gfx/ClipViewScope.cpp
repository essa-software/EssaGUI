#include "ClipViewScope.hpp"

namespace Gfx {

ClipViewScope::ClipViewScope(GUI::Window& target, Util::Rectf rect, Mode mode)
    : m_target(target)
    , m_old_projection(target.projection()) {

    auto old_viewport = m_old_projection.viewport();
    auto clip_rect = [&]() {
        switch (mode) {
        case Mode::Override:
            return rect;
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

    if (mode == Mode::Intersect) {
        Util::Vector2f offset_position { clip_rect.left - rect.left, clip_rect.top - rect.top };

        // TODO: Add some kind of move()
        clip_rect.left += offset_position.x();
        clip_rect.top += offset_position.y();
    }

    auto clip_view = create_clip_view({ clip_rect.left, clip_rect.top, clip_rect.width, clip_rect.height });
    m_target.set_projection(clip_view);
}

ClipViewScope::~ClipViewScope() {
    m_target.set_projection(m_old_projection);
}

llgl::Projection ClipViewScope::create_clip_view(Util::Rectf const& rect) const {
    return llgl::Projection::ortho({ { 0, 0, rect.width, rect.height } }, Util::Recti { rect });
}

}
