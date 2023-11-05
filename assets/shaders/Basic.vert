// Basic VS
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
