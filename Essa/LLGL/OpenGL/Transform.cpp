#include "Transform.hpp"
#include "Essa/AbstractOpenGLHelper.hpp"

#include "Error.hpp"

#include <iostream>

namespace llgl {

void set_viewport(Util::Recti rect) { OpenGL::Viewport(rect.left, rect.top, rect.width, rect.height); }

}
