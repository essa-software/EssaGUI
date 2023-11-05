// Test FS
#version 330 core

in vec4 fColor;
in vec2 fTexCoords;

uniform sampler2D texture;
uniform bool textureSet;
uniform float time;

void main() {
    float x = (sin(gl_FragCoord.x / 10.f + time) + 1.f) / 4.f;
    float y = (sin(gl_FragCoord.y / 10.f + time) + 1.f) / 4.f;
    gl_FragColor = fColor * (x + y);
}
