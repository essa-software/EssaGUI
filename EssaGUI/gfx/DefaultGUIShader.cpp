#include "DefaultGUIShader.hpp"

#include <EssaUtil/DelayedInit.hpp>
#include <iostream>

namespace Gfx {

char const VertexShader[] = R"~~~(// Default GUI VS
#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 texCoords;

uniform mat4 viewMatrix;

out vec4 fColor;
out vec4 fTexCoords;

void main() {
    fColor = color;
    fTexCoords = texCoords;
    gl_Position = viewMatrix * position;
}
)~~~";

char const FragmentShader[] = R"~~~(// Default GUI FS
#version 330 core

in vec4 fColor;
in vec4 fTexCoords;

uniform sampler2D texture;
uniform bool useTexture;

void main() {
    gl_FragColor = useTexture ? fColor * texture2D(texture, fTexCoords.xy) : fColor;
}
)~~~";

static Util::DelayedInit<sf::Shader> s_shader;

sf::Shader& default_gui_shader() {
    if (!s_shader.is_initialized()) {
        s_shader.construct();
        std::cerr << "EssaGUI: Loading GUI shader" << std::endl;
        if (!s_shader->loadFromMemory(VertexShader, FragmentShader)) {
            std::cerr << "EssaGUI: Failed to load GUI shader" << std::endl;
            exit(1);
        }
    }
    return *s_shader;
}

}
