#include "Basic.hpp"

#include <EssaUtil/Config.hpp>

namespace Essa::Shaders {

static std::string_view VertexShader = R"~~~(
#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 texCoord;
out vec4 f_color;
out vec2 f_texCoord;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main()
{
    mat4 matrix = projectionMatrix * viewMatrix * modelMatrix;
    f_color = color;
    f_texCoord = texCoord;
    gl_Position = matrix * vec4(position, 1);
}
)~~~";
static std::string_view FragmentShader = R"~~~(
#version 410 core

in vec4 f_color;
in vec2 f_texCoord;
uniform sampler2D texture;
uniform bool textureSet;

void main()
{
    vec4 color = f_color;
    if (textureSet)
        color *= texture2D(texture, f_texCoord);
    gl_FragColor = color;
}
)~~~";

std::string_view Basic::source(llgl::ShaderType type) const {
    switch (type) {
    case llgl::ShaderType::Vertex:
        return VertexShader;
    case llgl::ShaderType::Fragment:
        return FragmentShader;
    }
    ESSA_UNREACHABLE;
}

}
