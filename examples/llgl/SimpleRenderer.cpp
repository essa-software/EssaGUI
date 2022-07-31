#include <EssaUtil/Color.hpp>
#include <LLGL/OpenGL/Shaders/Basic330Core.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <LLGL/OpenGL/Utils.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/View.hpp>
#include <LLGL/Renderer/Renderer.hpp>
#include <LLGL/Window/Window.hpp>

int main()
{
    llgl::Window window { { 500, 500 }, "simple renderer" };

    llgl::opengl::shaders::Basic330Core shader;

    llgl::opengl::VAO vao { { { llgl::Vertex { { 150, 100, 0 }, Util::Colors::Green },
        llgl::Vertex { { 200, 200, 0 }, Util::Colors::Red },
        llgl::Vertex { { 100, 200, 0 }, Util::Colors::Blue } } } };

    for (;;) {
        llgl::Event event;
        while (window.poll_event(event)) {
        }

        llgl::opengl::disable(llgl::opengl::Feature::ScissorTest);
        window.renderer().clear(Util::Color { 255, 128, 128 });
        llgl::opengl::enable(llgl::opengl::Feature::ScissorTest);

        llgl::View view;
        view.set_viewport(window.rect());
        view.set_ortho(Util::Rectd { 0, 0, static_cast<double>(window.size().x()), static_cast<double>(window.size().y()) });
        window.renderer().apply_view(view);

        llgl::opengl::set_scissor({ 0, 0, 200, 200 });
        window.renderer().draw_vao(vao, llgl::opengl::PrimitiveType::Triangles, { .shader = &shader });
        window.display();
    }
    return 0;
}
