#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <EssaUtil/Clock.hpp>

class Shader : public llgl::Shader {
public:
    using Vertex = Gfx::Vertex;

    struct Uniforms : public Gfx::DefaultGUIShader::Uniforms {
        float time = 0;

        static inline auto mapping = Gfx::DefaultGUIShader::Uniforms::mapping | llgl::Uniform { "time", &Uniforms::time };
    };

    std::string_view source(llgl::ShaderType type) const {
        std::string_view fragment_shader = R"~~~(// Test FS
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
        )~~~";
        std::string_view vertex_shader = R"~~~(// Test VS
#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 submodelMatrix;

out vec4 fColor;
out vec2 fTexCoords;

void main() {
    fColor = color;
    fTexCoords = texCoords;
    gl_Position = projectionMatrix * (viewMatrix * (modelMatrix * (submodelMatrix * vec4(position, 0, 1))));
}
        )~~~";
        switch (type) {
        case llgl::ShaderType::Fragment:
            return fragment_shader;
        case llgl::ShaderType::Vertex:
            return vertex_shader;
        }
        ESSA_UNREACHABLE;
    }
};

class MainWidget : public GUI::Widget {
private:
    virtual void draw(Gfx::Painter& painter) const override {
        using namespace Gfx::Drawing;
        using namespace std::chrono_literals;
        painter.draw(Rectangle(local_rect().inflated(-10).cast<float>(), Fill::solid(Util::Colors::Red))
                         .set_shader(m_shader, { .time = static_cast<float>(m_clock.elapsed() / 100.0ms) }));
    }

    mutable Shader m_shader;
    Util::Clock m_clock;
};

int main() {
    GUI::SimpleApplication<MainWidget> app("Shaders");
    app.window().set_background_color(Util::Colors::Black);
    app.run();
    return 0;
}
