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
