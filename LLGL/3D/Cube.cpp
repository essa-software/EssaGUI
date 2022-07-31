#include "Cube.hpp"
#include "LLGL/3D/Shapes.hpp"

#include <LLGL/Renderer/Renderer.hpp>

namespace llgl {

Cube::Cube()
{
    generate();
}

void Cube::generate()
{
    std::vector<Vertex> vertices;
    Shapes::add_cube(vertices);
    m_vao.load_vertexes(vertices);
}

void Cube::render(Renderer& renderer, DrawState state) const
{
    renderer.draw_vao(m_vao, opengl::PrimitiveType::Triangles, state);
}

}
