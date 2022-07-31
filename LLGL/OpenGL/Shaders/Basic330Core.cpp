#include "Basic330Core.hpp"
#include "LLGL/OpenGL/Shader.hpp"

#include <EssaUtil/DelayedInit.hpp>

namespace llgl::opengl::shaders {

static Program& basic_330_core()
{
    static Util::DelayedInit<Program> program;
    if (!program.is_initialized()) {
        static char const* VERTEX_SHADER = R"~~~(
#version 410 core

layout(location=1) in vec4 position;
layout(location=2) in vec4 color;
layout(location=3) in vec2 texCoord;
layout(location=4) in vec3 normal;
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
    gl_Position = matrix * position;
}
)~~~";
        static char const* FRAGMENT_SHADER = R"~~~(
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
        auto objects = {
            ShaderObject { VERTEX_SHADER, ShaderObject::Vertex },
            ShaderObject { FRAGMENT_SHADER, ShaderObject::Fragment }
        };
        program.construct(objects);
    }
    return *program.ptr();
}

Basic330Core::Basic330Core()
    : Shader(basic_330_core())
{
}

AttributeMapping Basic330Core::attribute_mapping() const
{
    return {
        .position = 1,
        .color = 2,
        .tex_coord = 3,
        .normal = 4,
    };
}

void Basic330Core::on_bind(ShaderScope&) const
{
    // Nothing to do
}

}
