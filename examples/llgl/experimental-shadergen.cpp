#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/Error.hpp>
#include <Essa/LLGL/OpenGL/Extensions.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Vector.hpp>
#include <fmt/ostream.h>

auto VertexShader =
    R"~~~(
#version 410 core

layout(location=0) in vec2 position;
layout(location=1) in vec4 color;
out vec4 f_color;
uniform mat4 matrix;

void main()
{
    f_color = color;
    gl_Position = matrix * vec4(position,0,1);
}
)~~~";

auto FragmentShader =
    R"~~~(
#version 410 core

in vec4 f_color;
uniform vec4 otherColor;

void main()
{
    gl_FragColor = f_color + otherColor;
}    
)~~~";

struct MyShader : public llgl::Shader {
    using Vertex = llgl::Vertex<Util::Vector2f, Util::Colorf>;

    struct Uniforms {
        Util::Color other_color;
        Util::Matrix4x4f matrix;

        static inline auto mapping = llgl::make_uniform_mapping(
            llgl::Uniform { "otherColor", &Uniforms::other_color }, llgl::Uniform { "matrix", &Uniforms::matrix }
        );
    };

    std::string_view source(llgl::ShaderType type) {
        switch (type) {
        case llgl::ShaderType::Fragment:
            return FragmentShader;
        case llgl::ShaderType::Vertex:
            return VertexShader;
        }
        ESSA_UNREACHABLE;
    }
};

int main() {
    llgl::Window window { { 400, 400 }, "test" };
    llgl::opengl::enable_debug_output();

    llgl::VertexArray<MyShader::Vertex> varr;
    MyShader::Vertex verts[] {
        { { 50, 50 }, Util::Colors::Red },
        { { 100, 50 }, Util::Colors::Green },
        { { 50, 100 }, Util::Colors::Blue },
    };
    varr.upload_vertices(verts);

    auto& renderer = window.renderer();

    MyShader shader;

    MyShader::Uniforms uniforms;
    uniforms.other_color = Util::Colors::Red;
    uniforms.matrix = llgl::Transform {}.rotate_z(0.1).matrix() * llgl::Projection::ortho({ { 0, 0, 400, 400 } }, {}).matrix();

    while (true) {
        // FIXME: Port to GUI::Application
        while (window.poll_event()) { }
        llgl::set_viewport({ 0, 0, 400, 400 });
        renderer.clear();
        renderer.draw_vertices(varr, llgl::DrawState { shader, uniforms, llgl::PrimitiveType::Triangles });
        window.display();
    }
}
