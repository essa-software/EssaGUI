#pragma once

#include <EssaUtil/Rect.hpp>

#include <GL/glew.h>

namespace llgl::opengl {

void set_viewport(Util::Recti);

enum class MatrixMode {
    Projection = GL_PROJECTION
};
void set_matrix_mode(MatrixMode);

void load_identity();

struct OrthoArgs {
    OrthoArgs() = default;
    OrthoArgs(Util::Rectd rect, double near_ = 0, double far_ = 1)
        : left(rect.left)
        , right(rect.left + rect.width)
        , bottom(rect.top + rect.height)
        , top(rect.top)
        , near(near_)
        , far(far_)
    {
    }
    double left, right, bottom, top, near, far;
};
void load_ortho(OrthoArgs);

struct PerspectiveArgs {
    PerspectiveArgs() = default;
    PerspectiveArgs(double fov_, double aspect_, double near_, double far_)
        : fov(fov_)
        , aspect(aspect_)
        , near(near_)
        , far(far_)
    {
    }
    double fov, aspect, near, far;
};
void load_perspective(PerspectiveArgs);

void push_matrix();
void pop_matrix();

}
