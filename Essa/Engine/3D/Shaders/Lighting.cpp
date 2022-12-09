#include "Lighting.hpp"

namespace Essa::Shaders {

static std::string_view VertexShader = R"~~~(
#version 410 core
layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 texCoord;
layout(location=3) in vec3 normal;
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
    f_position = position;
    f_normal = normal;
    f_color = color;
    f_texCoord = texCoord;
    
    gl_Position = matrix * vec4(f_position, 1.0);
}
)~~~";

static std::string_view FragmentShader = R"~~~(
#version 410 core
in vec3 f_position;
in vec4 f_color;
in vec2 f_texCoord;
in vec3 f_normal;
uniform sampler2D texture;
uniform bool textureSet;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform vec3 lightPosition;
uniform vec4 lightColor;

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 emissionColor;

void main()
{
    vec3 lightPosVS = vec3(inverse(viewMatrix * modelMatrix) * vec4(lightPosition, 1));
    // TODO: Make all the factors configurable and not picked up from nowhere
    const float AmbientAmount = 0.2;
    const float MinDiffuse = 0.3;

    vec4 ambient = AmbientAmount * ambientColor * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(f_normal);
    vec3 lightDir = normalize(lightPosVS - f_position);
    float diff = min(max((dot(norm, lightDir) + 1) / 2, MinDiffuse), 1);
    vec4 diffuse = diff * diffuseColor * lightColor;
        
    vec4 result = (ambient + diffuse) * f_color;
    result += emissionColor;
    if (textureSet)
        result *= texture2D(texture, f_texCoord);
    gl_FragColor = vec4(result.rgb, 1.0);
}
)~~~";

std::string_view Lighting::source(llgl::ShaderType type) const {
    switch (type) {
    case llgl::ShaderType::Vertex:
        return VertexShader;
    case llgl::ShaderType::Fragment:
        return FragmentShader;
    }
    ESSA_UNREACHABLE;
}

}
