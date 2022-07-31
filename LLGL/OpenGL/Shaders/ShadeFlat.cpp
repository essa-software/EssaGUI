#include "ShadeFlat.hpp"

#include <EssaUtil/DelayedInit.hpp>

namespace llgl::opengl::shaders
{

static Program& shade_flat()
{
    static Util::DelayedInit<Program> shader;
    if (!shader.is_initialized())
    {
        static char const* VERTEX_SHADER = R"~~~(
#version 410 core
layout(location=1) in vec4 position;
layout(location=2) in vec4 color;
layout(location=3) in vec2 texCoord;
layout(location=4) in vec3 normal;
out vec3 f_position;
out vec4 f_color;
out vec2 f_texCoord;
out vec3 f_normal;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main()
{
    mat4 matrix = projectionMatrix * viewMatrix * modelMatrix;
    f_position = vec3(position);
    f_normal = normal;
    f_color = color;
    f_texCoord = texCoord;
    
    gl_Position = matrix * vec4(f_position, 1.0);
}
)~~~";
        static char const* FRAGMENT_SHADER = R"~~~(
#version 410 core
in vec3 f_position;
in vec4 f_color;
in vec2 f_texCoord;
in vec3 f_normal;
uniform sampler2D texture;
uniform bool textureSet;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform vec3 lightPos;
uniform vec4 lightColor;

void main()
{
    vec3 lightPosVS = vec3(inverse(viewMatrix * modelMatrix) * vec4(lightPos, 1));
    float ambientStrength = 0.2; // TODO: Make it configurable
    vec4 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(f_normal);
    vec3 lightDir = normalize(lightPosVS - f_position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * lightColor;
        
    vec4 result = (ambient + diffuse) * f_color;
    gl_FragColor = vec4(result.xyz, 1.0);
}
)~~~";
        auto objects = {
            ShaderObject { VERTEX_SHADER, ShaderObject::Vertex },
            ShaderObject { FRAGMENT_SHADER, ShaderObject::Fragment }
        };
        shader.construct(objects);
    }
    return *shader.ptr();
}

ShadeFlat::ShadeFlat()
    : Shader(shade_flat())
{
}

AttributeMapping ShadeFlat::attribute_mapping() const
{
    return {
        .position = 1,
        .color = 2,
        .tex_coord = 3,
        .normal = 4,
    };
}

void ShadeFlat::on_bind(ShaderScope& scope) const
{
    scope.set_uniform("lightPos", m_light_position);
    scope.set_uniform("lightColor", m_light_color);
}

}
