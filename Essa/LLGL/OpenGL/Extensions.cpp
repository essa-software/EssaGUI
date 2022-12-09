#include "Extensions.hpp"

#include <GL/glew.h>
#include <iostream>

namespace llgl::opengl
{

void ensure_glew()
{
    bool glew_initialized = false;
    auto rc = glewInit();
    if(!glew_initialized && rc != GLEW_OK)
    {
        std::cout << "GLEW: Initialization failed. " << (char*)glewGetErrorString(rc) << std::endl;
        abort();
    }
}

}
