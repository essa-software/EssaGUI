#include "Transform.hpp"

#include "Error.hpp"

#include <iostream>

namespace llgl {

void set_viewport(Util::Recti rect) { glViewport(rect.left, rect.top, rect.width, rect.height); }

}
