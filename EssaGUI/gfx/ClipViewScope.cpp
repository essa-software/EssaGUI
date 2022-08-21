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

    Util::Vector2f offset_position = (mode == Mode::Intersect ? Util::Vector2f { clip_rect.left - rect.left, clip_rect.top - rect.top } : Util::Vector2f {});

    auto clip_view = create_clip_view({ clip_rect.left, clip_rect.top, clip_rect.width, clip_rect.height }, offset_position);
    m_target.set_projection(clip_view);
}

ClipViewScope::~ClipViewScope() {
    m_target.set_projection(m_old_projection);
}

llgl::Projection ClipViewScope::create_clip_view(Util::Rectf const& rect, Util::Vector2f offset_position) const {
    return llgl::Projection::ortho({ { offset_position.x(), offset_position.y(), rect.width, rect.height } }, Util::Recti { rect });
}

}
