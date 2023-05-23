#pragma once

#include "OpenGL.hpp"
#include <EssaUtil/Rect.hpp>

namespace llgl {

void set_viewport(Util::Recti);

struct OrthoArgs {
    OrthoArgs() = default;
    OrthoArgs(Util::Rectd rect, double near_ = 0, double far_ = 1)
        : left(rect.left)
        , right(rect.left + rect.width)
        , bottom(rect.top + rect.height)
        , top(rect.top)
        , near(near_)
        , far(far_) { }
    double left, right, bottom, top, near, far;
};

struct PerspectiveArgs {
    PerspectiveArgs() = default;
    PerspectiveArgs(double fov_, double aspect_, double near_, double far_)
        : fov(fov_)
        , aspect(aspect_)
        , near(near_)
        , far(far_) { }
    double fov, aspect, near, far;
};

}
