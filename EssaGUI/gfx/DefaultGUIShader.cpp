#include "DefaultGUIShader.hpp"

#include <EssaUtil/DelayedInit.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <iostream>

namespace Gfx {

char const VertexShader[] = R"~~~(// Default GUI VS
#version 330 core

layout (location = 1) in vec4 position;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texCoords;

uniform mat4 projectionMatrix;

out vec4 fColor;
out vec2 fTexCoords;

void main() {
    fColor = color;
    fTexCoords = texCoords;
    gl_Position = projectionMatrix * position;
}
)~~~";

char const FragmentShader[] = R"~~~(// Default GUI FS
#version 330 core

in vec4 fColor;
in vec2 fTexCoords;

uniform sampler2D texture;
uniform bool textureSet;

void main() {
    gl_FragColor = textureSet ? fColor * texture2D(texture, fTexCoords.xy) : fColor;
}
)~~~";

static Util::DelayedInit<llgl::opengl::Program> s_shader;

llgl::opengl::Program& default_gui_shader() {
    if (!s_shader.is_initialized()) {
        std::cerr << "EssaGUI: Loading GUI shader" << std::endl;
        auto objects = {
            llgl::opengl::ShaderObject { VertexShader, llgl::opengl::ShaderObject::Type::Vertex },
            llgl::opengl::ShaderObject { FragmentShader, llgl::opengl::ShaderObject::Type::Fragment }
        };
        s_shader.construct(objects);
        if (!s_shader->valid()) {
            std::cerr << "EssaGUI: Failed to load GUI Shader." << std::endl;
            exit(-1);
        }
    }
    return *s_shader;
}

void DefaultGUIShader::on_bind(llgl::opengl::ShaderScope&) const {
    // Nothing to do, everything we need is handled by LLGL :)
    return;
}

}
