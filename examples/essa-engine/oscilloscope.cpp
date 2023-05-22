#include <Essa/Engine/3D/Shaders/Basic.hpp>
#include <Essa/LLGL/OpenGL/Error.hpp>
#include <Essa/LLGL/OpenGL/FBO.hpp>
#include <Essa/LLGL/OpenGL/Framebuffer.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/DelayedInit.hpp>
#include <EssaUtil/Vector.hpp>
#include <iostream>

class BlurShader : public llgl::Shader {
public:
    using Vertex = llgl::Vertex<Util::Vector3f, Util::Colorf, Util::Vector2f>;

    struct Uniforms {
    private:
        llgl::TextureUnit accum { 0 };
        llgl::TextureUnit pass1 { 1 };
        Util::Vector2f m_framebuffer_size;

    public:
        static inline auto mapping = llgl::make_uniform_mapping(
            llgl::Uniform("accum", &Uniforms::accum), llgl::Uniform("pass1", &Uniforms::pass1),
            llgl::Uniform("fbSize", &Uniforms::m_framebuffer_size)
        );

        void set_framebuffer_size(Util::Vector2f size) { m_framebuffer_size = size; }

        void set_accum(llgl::Texture const* tex) { accum.texture = tex; }
        void set_pass1(llgl::Texture const* tex) { pass1.texture = tex; }
    };

    auto source(llgl::ShaderType type) {
        switch (type) {
        case llgl::ShaderType::Fragment:
            return R"~~~(// BlurShader fs
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
        case llgl::ShaderType::Vertex:
            return R"~~~(// BlurShader vs
                #version 410 core

                layout(location=0) in vec4 position;

                out vec4 f_Position;

                void main()
                {
                    f_Position = position;
                    gl_Position = position;
                }
                )~~~";
        }
        ESSA_UNREACHABLE;
    };
};

Util::Cs::Point2f next_oscilloscope_position() {
    static float angle = 0;
    angle += 0.7;
    return Util::Cs::Point2f { std::sin(angle), std::cos(angle) } * 20 + llgl::mouse_position().to_vector().cast<float>();
}

int main() {
    llgl::Window window({ 512, 512 }, "Oscilloscope");
    llgl::opengl::enable_debug_output();

    Essa::Shaders::Basic basic_shader;
    BlurShader blur_shader;

    auto& renderer = window.renderer();

    llgl::Framebuffer pass1 { { 512, 512 } };
    pass1.set_label("pass1");
    llgl::Framebuffer accum { { 512, 512 } };
    accum.set_label("accum");

    llgl::VertexArray<Essa::Shaders::Basic::Vertex> fullscreen_vao {
        { { -1, -1, 0 }, Util::Colors::White, { 0, 1 } },
        { { 1, -1, 0 }, Util::Colors::White, { 1, 1 } },
        { { -1, 1, 0 }, Util::Colors::White, { 0, 0 } },
        { { 1, 1, 0 }, Util::Colors::White, { 1, 0 } },
    };

    auto old_oscilloscope_position = next_oscilloscope_position();

    for (;;) {
        // FIXME: Port to GUI::Application
        while (window.poll_event()) { }

        renderer.clear();

        BlurShader::Uniforms blur_shader_uniforms;
        blur_shader_uniforms.set_framebuffer_size(window.size().cast<float>().to_deprecated_vector());
        pass1.resize(window.size());
        accum.resize(window.size());

        {
            // Draw the first (non-blurred) pass
            pass1.clear();
            auto oscilloscope_position = next_oscilloscope_position();

            constexpr float PointSize = 2;
            auto diff = oscilloscope_position - old_oscilloscope_position;
            if (diff.length_squared() < PointSize * PointSize)
                diff = { PointSize, 0 };
            auto diff_norm = diff.normalized();
            auto cross = diff_norm.perpendicular() * PointSize;

            llgl::VertexArray<BlurShader::Vertex> input_vao {
                { Util::Vector3f((old_oscilloscope_position - cross).to_deprecated_vector(), 0), Util::Colors::Green, {} },
                { Util::Vector3f((old_oscilloscope_position + cross).to_deprecated_vector(), 0), Util::Colors::Green, {} },
                { Util::Vector3f((old_oscilloscope_position + diff - cross).to_deprecated_vector(), 0), Util::Colors::Green, {} },
                { Util::Vector3f((old_oscilloscope_position + diff + cross).to_deprecated_vector(), 0), Util::Colors::Green, {} }
            };

            Essa::Shaders::Basic::Uniforms basic_shader_uniforms;
            basic_shader_uniforms.set_transform(
                {}, {},
                llgl::Projection::ortho({ { 0, 0, static_cast<double>(window.size().x()), static_cast<double>(window.size().y()) } }, {})
                    .matrix()
            );
            pass1.draw_vertices(input_vao, llgl::DrawState { basic_shader, basic_shader_uniforms, llgl::PrimitiveType::TriangleStrip });

            old_oscilloscope_position = oscilloscope_position;
        }

        {
            // Blur the pass1 and blend with accum
            // Do not clear because we want previous frames
            blur_shader_uniforms.set_accum(&accum.color_texture());
            blur_shader_uniforms.set_pass1(&pass1.color_texture());
            accum.draw_vertices(fullscreen_vao, llgl::DrawState { blur_shader, blur_shader_uniforms, llgl::PrimitiveType::TriangleStrip });
        }

        // Draw the result to backbuffer

        llgl::set_viewport(window.rect());
        Essa::Shaders::Basic::Uniforms basic_shader_uniforms;
        basic_shader_uniforms.set_texture(&accum.color_texture());
        basic_shader_uniforms.set_transform({}, {}, llgl::Projection::ortho({ { -1, -1, 2, 2 } }, {}).matrix());
        renderer.draw_vertices(fullscreen_vao, llgl::DrawState { basic_shader, basic_shader_uniforms, llgl::PrimitiveType::TriangleStrip });
        window.display();
    }
    return 0;
}
