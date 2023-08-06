#pragma once

#include <GL/glew.h>

#include <GL/gl.h>
#include <array>
#include <functional>
#include <list>
#include <string>
#include <tuple>
#include <vector>

#define ADD_OPENGL_FUNCTION(FuncName) \
    template<typename... Args> static auto FuncName(Args... args) { return gl##FuncName(args...); }

class OpenGL {
public:
    ADD_OPENGL_FUNCTION(ActiveTexture)
    ADD_OPENGL_FUNCTION(BindTexture)
    ADD_OPENGL_FUNCTION(DeleteTextures)
    ADD_OPENGL_FUNCTION(GenTextures)
    ADD_OPENGL_FUNCTION(TexImage2D)
    ADD_OPENGL_FUNCTION(FramebufferTexture)
    ADD_OPENGL_FUNCTION(GetTexImage)

    ADD_OPENGL_FUNCTION(BufferData)
    ADD_OPENGL_FUNCTION(BindBuffer)
    ADD_OPENGL_FUNCTION(BindFramebuffer)
    ADD_OPENGL_FUNCTION(BindRenderbuffer)
    ADD_OPENGL_FUNCTION(DrawBuffers)
    ADD_OPENGL_FUNCTION(DeleteBuffers)
    ADD_OPENGL_FUNCTION(DeleteFramebuffers)
    ADD_OPENGL_FUNCTION(DeleteRenderbuffers)
    ADD_OPENGL_FUNCTION(FramebufferRenderbuffer)
    ADD_OPENGL_FUNCTION(GenFramebuffers)
    ADD_OPENGL_FUNCTION(GenRenderbuffers)
    ADD_OPENGL_FUNCTION(GenBuffers)
    ADD_OPENGL_FUNCTION(CheckFramebufferStatus)
    ADD_OPENGL_FUNCTION(RenderbufferStorage)

    ADD_OPENGL_FUNCTION(BindVertexArray)
    ADD_OPENGL_FUNCTION(DeleteVertexArrays)
    ADD_OPENGL_FUNCTION(DrawElements)
    ADD_OPENGL_FUNCTION(DrawArrays)
    ADD_OPENGL_FUNCTION(GenVertexArrays)
    ADD_OPENGL_FUNCTION(VertexAttribPointer)
    ADD_OPENGL_FUNCTION(EnableVertexAttribArray)

    ADD_OPENGL_FUNCTION(Clear)
    ADD_OPENGL_FUNCTION(ClearColor)
    ADD_OPENGL_FUNCTION(Viewport)

    ADD_OPENGL_FUNCTION(Enable)
    ADD_OPENGL_FUNCTION(Disable)
    ADD_OPENGL_FUNCTION(BlendFuncSeparate)
    ADD_OPENGL_FUNCTION(GetError)

    ADD_OPENGL_FUNCTION(CreateShader)
    ADD_OPENGL_FUNCTION(CompileShader)
    ADD_OPENGL_FUNCTION(DeleteShader)
    ADD_OPENGL_FUNCTION(GetShaderiv)
    ADD_OPENGL_FUNCTION(GetShaderInfoLog)
    ADD_OPENGL_FUNCTION(ShaderSource)

    ADD_OPENGL_FUNCTION(CreateProgram)
    ADD_OPENGL_FUNCTION(DeleteProgram)
    ADD_OPENGL_FUNCTION(GetProgramiv)
    ADD_OPENGL_FUNCTION(GetProgramInfoLog)
    ADD_OPENGL_FUNCTION(LinkProgram)
    ADD_OPENGL_FUNCTION(ObjectLabel)
    ADD_OPENGL_FUNCTION(UseProgram)

    ADD_OPENGL_FUNCTION(GetUniformLocation)
    ADD_OPENGL_FUNCTION(Uniform1i)
    ADD_OPENGL_FUNCTION(Uniform4f)
    ADD_OPENGL_FUNCTION(UniformMatrix4fv)
    ADD_OPENGL_FUNCTION(Uniform2f)
    ADD_OPENGL_FUNCTION(Uniform3f)
    ADD_OPENGL_FUNCTION(Uniform1f)
};
