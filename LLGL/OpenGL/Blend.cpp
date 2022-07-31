#include "Blend.hpp"

#include "Error.hpp"

namespace llgl::opengl
{

void set_blend_func(BlendFunc source, BlendFunc destination)
{
    glBlendFunc((GLenum)source, (GLenum)destination);
}

}
