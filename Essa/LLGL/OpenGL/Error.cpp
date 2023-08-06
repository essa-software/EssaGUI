#include "Error.hpp"

#include "Extensions.hpp"

#include <GL/glew.h>

#include <GL/glu.h>
#include <iostream>
#include <Essa/LLGL/AbstractOpenGLHelper.hpp>

namespace llgl::opengl {

void enable_debug_output() {
    ensure_glew();
    OpenGL::Enable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        []([[maybe_unused]] GLenum source, GLenum type, [[maybe_unused]] GLuint id, [[maybe_unused]] GLenum severity,
           [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* user_param) {
            if (type == GL_DEBUG_TYPE_ERROR)
                std::cout << "GL Error: " << message << std::endl;
        },
        0
    );
}

void handle_error(std::source_location location) {
    if (OpenGL::GetError() != GL_NO_ERROR)
        std::cout << "TODO: remove this handler " << location.file_name() << ":" << location.line() << ":" << location.column()
                  << std::endl;
}

}
