#include "ClipViewScope.hpp"

namespace Gfx {

ClipViewScope::ClipViewScope(Gfx::Painter& target, Util::Vector2u host_window_size, Util::Rectf rect, Mode mode)
    : m_target(target)
    , m_old_projection(target.builder().projection()) {

    auto old_viewport = m_old_projection.viewport();
    old_viewport.top = host_window_size.y() - old_viewport.top - old_viewport.height;
    auto clip_rect = [&]() {
        switch (mode) {
        case Mode::Override:
            return rect;
        case Mode::Intersect: {
            return Util::Recti { old_viewport }.intersection(rect);
        }
        }
        __builtin_unreachable();
    }();

    Util::Vector2f offset_position = (mode == Mode::Intersect ? Util::Vector2f { clip_rect.left - rect.left, clip_rect.top - rect.top } : Util::Vector2f {});

    auto clip_view = create_clip_view(clip_rect, offset_position, host_window_size);
    m_target.builder().set_projection(clip_view);
}

ClipViewScope::~ClipViewScope() {
    m_target.builder().set_projection(m_old_projection);
}

llgl::Projection ClipViewScope::create_clip_view(Util::Rectf const& rect, Util::Vector2f offset_position, Util::Vector2u host_window_size) const {
    return llgl::Projection::ortho({ { offset_position.x(), offset_position.y(), rect.width, rect.height } },
        Util::Recti { static_cast<int>(rect.left), static_cast<int>(host_window_size.y() - rect.top - rect.height), static_cast<int>(rect.width), static_cast<int>(rect.height) });
}

}
