#include "ClipViewScope.hpp"

#include <Essa/GUI/Graphics/Painter.hpp>

namespace Gfx {

static thread_local ClipViewScope* s_current = nullptr;

ClipViewScope::ClipViewScope(Gfx::Painter& target, Util::Recti rect, Mode mode)
    : m_target(target)
    , m_old_projection(target.projection())
    , m_parent(s_current) {
    s_current = this;

    auto old_viewport = m_old_projection.viewport();
    if (mode != Mode::NewStack) {
        rect = rect.move_x(old_viewport.left).move_y(old_viewport.top);
        if (m_parent) {
            rect = rect.move_y(-m_parent->m_offset.y());
        }
    }

    auto clip_rect = [&]() {
        switch (mode) {
        case Mode::Override:
        case Mode::NewStack:
            return rect;
        case Mode::Intersect: {
            return Util::Recti { old_viewport }.intersection(rect);
        }
        }
        __builtin_unreachable();
    }();

    Util::Vector2i offset_position
        = (mode == Mode::Intersect ? Util::Vector2i(clip_rect.left - rect.left, clip_rect.top - rect.top) : Util::Vector2i());

    m_offset = offset_position;

    auto clip_view = create_clip_view(clip_rect, offset_position);

    m_target.set_projection(clip_view);
}

ClipViewScope::~ClipViewScope() {
    m_target.set_projection(m_old_projection);
    s_current = m_parent;
}

llgl::Projection ClipViewScope::create_clip_view(Util::Recti const& rect, Util::Vector2i offset_position) {
    return llgl::Projection::ortho({ { offset_position.cast<double>().to_point(), rect.size().cast<double>() } }, rect);
}

}
