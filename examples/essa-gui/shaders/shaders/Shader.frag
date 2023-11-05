// Test FS
#version 330 core

in vec4 fColor;
in vec2 fTexCoords;

uniform sampler2D texture;
uniform bool textureSet;

uniform float time;
uniform float size;

void main() {
    float x = (sin((gl_FragCoord.x) * (1 - 1 / (size + 5)) / 2 + time) + 1.f) / 4;
    float y = (sin((gl_FragCoord.y) * (1 - 1 / (size + 5)) / 2 + time) + 1.f) / 4;
    gl_FragColor = fColor * (x + y);
}
