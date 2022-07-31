#include <EssaUtil/Color.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <LLGL/OpenGL/Utils.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/Window/Window.hpp>

int main()
{
    llgl::Window window{{500, 500}, "test"};
    llgl::opengl::set_clear_color(Util::Color{255, 128, 128});
    for(;;)
    {
        llgl::Event event;
        while(window.poll_event(event))
        {
        }
        llgl::opengl::clear(llgl::opengl::ClearMask::Color);
        llgl::opengl::set_matrix_mode(llgl::opengl::MatrixMode::Projection);
        llgl::opengl::load_identity();
        llgl::opengl::load_ortho(llgl::Rectd{0, 0, static_cast<double>(window.size().x), static_cast<double>(window.size().y)});
        llgl::opengl::set_viewport(llgl::Recti{0, 0, window.size().x, window.size().y});

        {
            llgl::opengl::DrawScope scope(llgl::opengl::PrimitiveType::Triangles);
            llgl::opengl::set_vertex_color({128, 255, 128});

            llgl::opengl::add_vertex({10, 10});
            llgl::opengl::add_vertex({20, 10});
            llgl::opengl::add_vertex({20, 20});

            llgl::opengl::add_vertex({10, 10});
            llgl::opengl::add_vertex({10, 20});
            llgl::opengl::add_vertex({20, 20});
        }

        window.display();
    }
    return 0;
}
