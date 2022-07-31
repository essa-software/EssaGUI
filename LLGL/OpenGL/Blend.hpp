#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

namespace llgl::opengl
{

enum class BlendFunc
{
    // TODO: Add all blend funces here
    Zero = GL_ZERO,
    One = GL_ONE,
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA
};

void set_blend_func(BlendFunc source, BlendFunc destination);

}
