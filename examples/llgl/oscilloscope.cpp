#include <EssaUtil/Color.hpp>
#include <EssaUtil/DelayedInit.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/OpenGL/Error.hpp>
#include <LLGL/OpenGL/FBO.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Shaders/Basic330Core.hpp>
#include <LLGL/OpenGL/Utils.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/Renderer/RenderToTexture.hpp>
#include <LLGL/Renderer/Transform.hpp>
#include <LLGL/Window/Mouse.hpp>
#include <LLGL/Window/Window.hpp>
#include <SDL2/SDL_video.h>
#include <iostream>

static llgl::opengl::Program& shader() {
    static Util::DelayedInit<llgl::opengl::Program> program;
    if (!program.is_initialized()) {
        static char const* VERTEX_SHADER = R"~~~(
#version 410 core

layout(location=1) in vec4 position;

out vec4 f_Position;

void main()
{
    f_Position = position;
    gl_Position = position;
}
)~~~";
        static char const* FRAGMENT_SHADER = R"~~~(
#version 410 core

uniform sampler2D accum;
uniform sampler2D pass1;
uniform vec2 fbSize;

in vec4 f_Position;

void main()
{
    vec2 fragPos = f_Position.st / 2 + vec2(0.5, 0.5);
    vec4 color;

    // Box blur
    for (int x = -3; x <= 3; x++) {
        for (int y = -3; y <= 3; y++) {
            color += texture2D(pass1, fragPos + vec2(x/fbSize.x, y/fbSize.y)) / 49;
        }
    }

    // Motion blur
    vec4 accumFactor = texture2D(accum, fragPos) * 0.9;
    if (accumFactor.r < 0.05)
        accumFactor.r = 0;
    if (accumFactor.g < 0.05)
        accumFactor.g = 0;
    if (accumFactor.b < 0.05)
        accumFactor.b = 0;
    gl_FragColor = color + accumFactor;
}
)~~~";
        auto objects = {
            llgl::opengl::ShaderObject { VERTEX_SHADER, llgl::opengl::ShaderObject::Vertex },
            llgl::opengl::ShaderObject { FRAGMENT_SHADER, llgl::opengl::ShaderObject::Fragment }
        };
        program.construct(objects);
    }
    return *program.ptr();
}

class BlurShader : public llgl::opengl::Shader {
public:
    BlurShader()
        : llgl::opengl::Shader(shader()) {
    }

    virtual llgl::opengl::AttributeMapping attribute_mapping() const override {
        return { .position = 1 };
    }

    void set_framebuffer_size(Util::Vector2f size) { m_framebuffer_size = size; }

private:
    virtual void on_bind(llgl::opengl::ShaderScope& scope) const override {
        scope.set_uniform("accum", llgl::opengl::ShaderScope::CurrentTexture);
        scope.set_uniform("pass1", 1);
        scope.set_uniform("fbSize", m_framebuffer_size);
    }

    Util::Vector2f m_framebuffer_size;
};

Util::Vector2f next_oscilloscope_position() {
    static float angle = 0;
    angle += 0.7;
    return Util::Vector2f { std::sin(angle), std::cos(angle) } * 20
        + Util::Vector2f { llgl::mouse_position() };
}

int main() {
    llgl::Window window({ 512, 512 }, "Oscilloscope");
    llgl::opengl::enable_debug_output();

    auto& renderer = window.renderer();
    llgl::opengl::shaders::Basic330Core basic_shader;
    BlurShader blur_shader;

    llgl::RenderToTexture pass1 { window };
    pass1.set_label("pass1");
    llgl::RenderToTexture accum { window };
    accum.set_label("accum");

    llgl::opengl::VAO fullscreen_vao;
    fullscreen_vao.load_vertexes(
        { { llgl::Vertex { .position = { -1, -1, 0 }, .color = Util::Colors::White, .tex_coord = { 0, 1 } },
            llgl::Vertex { .position = { 1, -1, 0 }, .color = Util::Colors::White, .tex_coord = { 1, 1 } },
            llgl::Vertex { .position = { -1, 1, 0 }, .color = Util::Colors::White, .tex_coord = { 0, 0 } },
            llgl::Vertex { .position = { 1, 1, 0 }, .color = Util::Colors::White, .tex_coord = { 1, 0 } } } });

    auto old_oscilloscope_position = next_oscilloscope_position();

    for (;;) {
        llgl::Event event;
        while (window.poll_event(event)) {
            switch (event.type) {
            default:
                break;
            }
        }

        renderer.clear(Util::Colors::Black);

        blur_shader.set_framebuffer_size(Util::Vector2f { window.size() });

        {
            // Draw the first (non-blurred) pass
            pass1.clear(Util::Color { 0, 0, 0 });
            pass1.apply_projection(llgl::Projection::ortho({ { 0, 0, static_cast<double>(window.size().x()), static_cast<double>(window.size().y()) } }, window.rect()));

            auto oscilloscope_position = next_oscilloscope_position();

            constexpr float PointSize = 2;
            auto diff = oscilloscope_position - old_oscilloscope_position;
            if (diff.length_squared() < PointSize * PointSize)
                diff = { PointSize, 0 };
            auto diff_norm = diff.normalized();
            auto cross = diff_norm.perpendicular() * PointSize;

            llgl::opengl::VAO input_vao {
                { { llgl::Vertex { .position = Util::Vector3f { old_oscilloscope_position - cross, 0 }, .color = Util::Colors::Green },
                    llgl::Vertex { .position = Util::Vector3f { old_oscilloscope_position + cross, 0 }, .color = Util::Colors::Green },
                    llgl::Vertex { .position = Util::Vector3f { old_oscilloscope_position + diff - cross, 0 }, .color = Util::Colors::Green },
                    llgl::Vertex { .position = Util::Vector3f { old_oscilloscope_position + diff + cross, 0 }, .color = Util::Colors::Green } } }
            };

            pass1.draw_vao(input_vao, llgl::opengl::PrimitiveType::TriangleStrip, { .shader = &basic_shader });

            old_oscilloscope_position = oscilloscope_position;
        }

        {
            // Blur the pass1 and blend with accum
            // Do not clear because we want previous frames

            // TODO: Implement this in LLGL
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pass1.texture().id());
            glActiveTexture(GL_TEXTURE0);

            accum.draw_vao(fullscreen_vao, llgl::opengl::PrimitiveType::TriangleStrip, { .shader = &blur_shader, .texture = &accum.texture() });
        }

        // Draw the result to backbuffer
        renderer.apply_projection({ {}, window.rect() });
        renderer.draw_vao(fullscreen_vao, llgl::opengl::PrimitiveType::TriangleStrip, { .shader = &basic_shader, .texture = &accum.texture() });
        window.display();
    }
    return 0;
}
