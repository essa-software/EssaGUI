#pragma once

#include "OpenGL.hpp"

namespace llgl {

enum class PrimitiveType {
    Invalid = -1,
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Triangles = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
};

}
