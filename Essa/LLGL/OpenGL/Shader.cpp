#include "Shader.hpp"

namespace llgl {

namespace Detail {

unsigned compile_shader(GLenum type, std::string_view source) {
    opengl::ensure_glew();
    auto id = glCreateShader(type);

    char const* sources[] = { source.data() };
    int const lengths[] = { static_cast<int>(source.size()) };
    glShaderSource(id, 1, sources, lengths);
    glCompileShader(id);

    GLint success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint max_length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);
        std::string error_message;
        error_message.resize(max_length);
        glGetShaderInfoLog(id, max_length, &max_length, error_message.data());
        std::cout << "Shader: Failed to compile: " << error_message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

}

}
