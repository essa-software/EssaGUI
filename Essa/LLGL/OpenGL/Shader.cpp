#include "Shader.hpp"

namespace llgl {

namespace Detail {

unsigned compile_shader(GLenum type, std::string_view source) {
    opengl::ensure_glew();
    auto id = OpenGL::CreateShader(type);

    char const* sources[] = { source.data() };
    int const lengths[] = { static_cast<int>(source.size()) };
    OpenGL::ShaderSource(id, 1, sources, lengths);
    OpenGL::CompileShader(id);

    GLint success = 0;
    OpenGL::GetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint max_length = 0;
        OpenGL::GetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);
        std::string error_message;
        error_message.resize(max_length);
        OpenGL::GetShaderInfoLog(id, max_length, &max_length, error_message.data());
        std::cout << "Shader: Failed to compile: " << error_message << std::endl;
        OpenGL::DeleteShader(id);
        return 0;
    }
    return id;
}

}

}
