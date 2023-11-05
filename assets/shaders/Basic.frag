// Basic FS
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
