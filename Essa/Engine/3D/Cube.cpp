#include "Cube.hpp"

#include "Shapes.hpp"

namespace Essa {

Cube::Cube() {
    generate();
}

void Cube::generate() {
    std::vector<Model::Vertex> vertices;
    Shapes::add_cube(vertices);
    m_vao.upload_vertices(vertices);
}

}
