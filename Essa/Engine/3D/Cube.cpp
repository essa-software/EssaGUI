#include "Cube.hpp"

#include "Shapes.hpp"

namespace Essa {

Cube::Cube(RenderingMode mode)
    : m_rendering_mode(mode) {
    generate();
}

void Cube::generate() {
    std::vector<Model::Vertex> vertices;
    switch (m_rendering_mode) {
    case RenderingMode::Solid:
        Shapes::add_cube(vertices);
        break;
    case RenderingMode::Wireframe:
        Shapes::add_wireframe_cube(vertices);
        break;
    }
    m_vao.upload_vertices(vertices);
}

}
