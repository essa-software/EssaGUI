#pragma once
#include <GL/gl.h>
#include <GL/glew.h>

#include <array>
#include <functional>
#include <list>
#include <string>
#include <tuple>
#include <vector>

struct OpenGLCommand {
    std::string name;
    void* params = nullptr;
    std::function<void(void*)> destroy;
};

#define ADD_OPENGL_FUNCTION(FuncName, ShouldWrite)                                                             \
    template<typename... Args> static auto FuncName(Args... args) {                                            \
        if (ShouldWrite && write_commands_to_buffer) {                                                         \
            OpenGLCommand cmd { .name = "gl" + std::string(#FuncName),                                         \
                                .params = new std::tuple<Args...>(args...),                                    \
                                .destroy = [](void* ptr) { delete static_cast<std::tuple<Args...>*>(ptr); } }; \
            command_buffer.push_back(cmd);                                                                     \
        }                                                                                                      \
        return gl##FuncName(args...);                                                                          \
    }

class OpenGL {
public:
    static std::list<OpenGLCommand> command_buffer;
    static bool write_commands_to_buffer;

    ADD_OPENGL_FUNCTION(ActiveTexture, true)
    ADD_OPENGL_FUNCTION(BindTexture, true)
    ADD_OPENGL_FUNCTION(DeleteTextures, true)
    ADD_OPENGL_FUNCTION(GenTextures, true)
    ADD_OPENGL_FUNCTION(TexImage2D, false)
    ADD_OPENGL_FUNCTION(FramebufferTexture, false)
    ADD_OPENGL_FUNCTION(GetTexImage, true)

    ADD_OPENGL_FUNCTION(BufferData, true)
    ADD_OPENGL_FUNCTION(BindBuffer, true)
    ADD_OPENGL_FUNCTION(BindFramebuffer, true)
    ADD_OPENGL_FUNCTION(BindRenderbuffer, true)
    ADD_OPENGL_FUNCTION(DrawBuffers, true)
    ADD_OPENGL_FUNCTION(DeleteBuffers, true)
    ADD_OPENGL_FUNCTION(DeleteFramebuffers, true)
    ADD_OPENGL_FUNCTION(DeleteRenderbuffers, true)
    ADD_OPENGL_FUNCTION(FramebufferRenderbuffer, true)
    ADD_OPENGL_FUNCTION(GenFramebuffers, true)
    ADD_OPENGL_FUNCTION(GenRenderbuffers, true)
    ADD_OPENGL_FUNCTION(GenBuffers, true)
    ADD_OPENGL_FUNCTION(CheckFramebufferStatus, false)
    ADD_OPENGL_FUNCTION(RenderbufferStorage, false)

    ADD_OPENGL_FUNCTION(BindVertexArray, true)
    ADD_OPENGL_FUNCTION(DeleteVertexArrays, true)
    ADD_OPENGL_FUNCTION(DrawElements, true)
    ADD_OPENGL_FUNCTION(DrawArrays, true)
    ADD_OPENGL_FUNCTION(GenVertexArrays, true)
    ADD_OPENGL_FUNCTION(VertexAttribPointer, true)
    ADD_OPENGL_FUNCTION(EnableVertexAttribArray, true)

    ADD_OPENGL_FUNCTION(Clear, true)
    ADD_OPENGL_FUNCTION(ClearColor, true)
    ADD_OPENGL_FUNCTION(Viewport, true)

    ADD_OPENGL_FUNCTION(Enable, true)
    ADD_OPENGL_FUNCTION(Disable, true)
    ADD_OPENGL_FUNCTION(BlendFuncSeparate, false)
    ADD_OPENGL_FUNCTION(GetError, false)

    ADD_OPENGL_FUNCTION(CreateShader, false)
    ADD_OPENGL_FUNCTION(CompileShader, false)
    ADD_OPENGL_FUNCTION(DeleteShader, false)
    ADD_OPENGL_FUNCTION(GetShaderiv, false)
    ADD_OPENGL_FUNCTION(GetShaderInfoLog, false)
    ADD_OPENGL_FUNCTION(ShaderSource, false)

    ADD_OPENGL_FUNCTION(CreateProgram, false)
    ADD_OPENGL_FUNCTION(DeleteProgram, false)
    ADD_OPENGL_FUNCTION(GetProgramiv, false)
    ADD_OPENGL_FUNCTION(GetProgramInfoLog, false)
    ADD_OPENGL_FUNCTION(LinkProgram, false)
    ADD_OPENGL_FUNCTION(ObjectLabel, false)
    ADD_OPENGL_FUNCTION(UseProgram, false)

    ADD_OPENGL_FUNCTION(GetUniformLocation, false)
    ADD_OPENGL_FUNCTION(Uniform1i, true)
    ADD_OPENGL_FUNCTION(Uniform4f, true)
    ADD_OPENGL_FUNCTION(UniformMatrix4fv, true)
    ADD_OPENGL_FUNCTION(Uniform2f, true)
    ADD_OPENGL_FUNCTION(Uniform3f, true)
    ADD_OPENGL_FUNCTION(Uniform1f, true)

    ADD_OPENGL_FUNCTION(XChooseFBConfig, false)
    ADD_OPENGL_FUNCTION(XGetVisualFromFBConfig, false)

    static auto clear() {
        for (auto& cmd : command_buffer) {
            cmd.destroy(cmd.params);
        }

        command_buffer.clear();
    }
};
