#include "DefaultGUIShader.hpp"

#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <EssaUtil/DelayedInit.hpp>
#include <iostream>

namespace Gfx {

#ifdef __EMSCRIPTEN__
static std::string_view VertexShader = R"~~~(// Default GUI VS
precision highp float;
precision highp int;

attribute vec2 position;
attribute vec4 color;
attribute vec2 texCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 submodelMatrix;

varying vec4 fColor;
varying vec2 fTexCoords;

void main() {
    fColor = color;
    fTexCoords = texCoords;
    gl_Position = projectionMatrix * (viewMatrix * (modelMatrix * (submodelMatrix * vec4(position, 0, 1))));
}
)~~~";

static std::string_view FragmentShader = R"~~~(// Default GUI FS
precision highp float;
precision highp int;

varying vec4 fColor;
varying vec2 fTexCoords;

uniform sampler2D texture;
uniform bool textureSet;

void main() {
    gl_FragColor = textureSet ? fColor * texture2D(texture, fTexCoords.xy) : fColor;
}
)~~~";
#else
static std::string_view VertexShader = R"~~~(// Default GUI VS
#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 submodelMatrix;

out vec4 fColor;
out vec2 fTexCoords;

void main() {
    fColor = color;
    fTexCoords = texCoords;
    gl_Position = projectionMatrix * (viewMatrix * (modelMatrix * (submodelMatrix * vec4(position, 0, 1))));
}
)~~~";

static std::string_view FragmentShader = R"~~~(// Default GUI FS
#version 330 core

in vec4 fColor;
in vec2 fTexCoords;

uniform sampler2D texture;
uniform bool textureSet;

void main() {
    gl_FragColor = textureSet ? fColor * texture2D(texture, fTexCoords.xy) : fColor;
}
)~~~";
#endif

std::string_view DefaultGUIShader::source(llgl::ShaderType type) const {
    switch (type) {
    case llgl::ShaderType::Vertex:
        return VertexShader;
    case llgl::ShaderType::Fragment:
        return FragmentShader;
    }
    ESSA_UNREACHABLE;
}

}
