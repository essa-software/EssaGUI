#include "Projection.hpp"

#include <iostream>

namespace llgl {

Projection Projection::ortho(OrthoArgs ortho, Util::Recti viewport) {
    float tx = -(ortho.right + ortho.left) / (ortho.right - ortho.left);
    float ty = -(ortho.top + ortho.bottom) / (ortho.top - ortho.bottom);
    float tz = -(ortho.far + ortho.near) / (ortho.far - ortho.near);
    return { { static_cast<float>(2.0f / (ortho.right - ortho.left)), 0.0f, 0.0f, tx, 0.0f,
               static_cast<float>(2.0f / (ortho.top - ortho.bottom)), 0.0f, ty, 0.0f, 0.0f,
               static_cast<float>(-2.0f / (ortho.far - ortho.near)), tz, 0.0f, 0.0f, 0.0f, 1.0f },
             viewport };
}

Projection Projection::perspective(PerspectiveArgs persp, Util::Recti viewport) {
    float f = 1.f / std::tan(persp.fov / 2);
    return { { static_cast<float>(f / persp.aspect), 0, 0, 0, 0, f, 0, 0, 0, 0,
               static_cast<float>((persp.far + persp.near) / (persp.near - persp.far)),
               static_cast<float>((2 * persp.far * persp.near) / (persp.near - persp.far)), 0, 0, -1, 0 },
             viewport };
}

}
