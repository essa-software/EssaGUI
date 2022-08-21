#include "Cube.hpp"

#include "Shapes.hpp"

#include <LLGL/Renderer/Renderer.hpp>

namespace Essa {

Cube::Cube() {
    generate();
}

void Cube::generate() {
    std::vector<llgl::Vertex> vertices;
    Shapes::add_cube(vertices);
    m_vao.load_vertexes(vertices);
}

void Cube::render(llgl::Renderer& renderer, llgl::DrawState state) const {
    renderer.draw_vao(m_vao, llgl::opengl::PrimitiveType::Triangles, state);
}

}
