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

unsigned link_shader(std::initializer_list<unsigned> ids) {
    for (auto id : ids) {
        assert(id != 0);
    }
    auto program_id = OpenGL::CreateProgram();
    for (auto id : ids) {
        glAttachShader(program_id, id);
    }

    // TODO: Attrib locations if supported
    OpenGL::LinkProgram(program_id);
    GLint success = 0;
    OpenGL::GetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint max_length = 0;
        OpenGL::GetProgramiv(program_id, GL_INFO_LOG_LENGTH, &max_length);
        std::string error_message;
        error_message.resize(max_length);
        OpenGL::GetProgramInfoLog(program_id, max_length, &max_length, error_message.data());
        std::cout << "Program: Failed to link shader program: " << error_message << std::endl;
        OpenGL::DeleteProgram(program_id);
        return 0;
    }

    for (auto id : ids) {
        glDetachShader(program_id, id);
    }
    return program_id;
}

}

}
