#include "Shapes.hpp"

namespace llgl::Shapes {

void add_cube(std::vector<Vertex>& vertices, CubeProperties properties)
{
    auto transform_point = [&](Util::Vector3f vec) {
        return properties.transform.transform_point(
            Util::Vector3f { vec.x() * properties.dimensions.x() / 2, vec.y() * properties.dimensions.y() / 2, vec.z() * properties.dimensions.z() / 2 }
            + properties.position);
    };

    // left
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, -1 }), .color = properties.color, .normal = { -1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, -1 }), .color = properties.color, .normal = { -1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, 1 }), .color = properties.color, .normal = { -1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, -1 }), .color = properties.color, .normal = { -1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, 1 }), .color = properties.color, .normal = { -1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, 1 }), .color = properties.color, .normal = { -1, 0, 0 } });

    // right
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, -1 }), .color = properties.color, .normal = { 1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, -1 }), .color = properties.color, .normal = { 1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, 1 }), .color = properties.color, .normal = { 1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, -1 }), .color = properties.color, .normal = { 1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, 1 }), .color = properties.color, .normal = { 1, 0, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, 1 }), .color = properties.color, .normal = { 1, 0, 0 } });

    // bottom
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, -1 }), .color = properties.color, .normal = { 0, -1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, -1 }), .color = properties.color, .normal = { 0, -1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, 1 }), .color = properties.color, .normal = { 0, -1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, -1 }), .color = properties.color, .normal = { 0, -1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, 1 }), .color = properties.color, .normal = { 0, -1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, 1 }), .color = properties.color, .normal = { 0, -1, 0 } });

    // top
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, -1 }), .color = properties.color, .normal = { 0, 1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, -1 }), .color = properties.color, .normal = { 0, 1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, 1 }), .color = properties.color, .normal = { 0, 1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, -1 }), .color = properties.color, .normal = { 0, 1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, 1 }), .color = properties.color, .normal = { 0, 1, 0 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, 1 }), .color = properties.color, .normal = { 0, 1, 0 } });

    // front
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, -1 }), .color = properties.color, .normal = { 0, 0, -1 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, -1 }), .color = properties.color, .normal = { 0, 0, -1 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, -1 }), .color = properties.color, .normal = { 0, 0, -1 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, -1 }), .color = properties.color, .normal = { 0, 0, -1 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, -1 }), .color = properties.color, .normal = { 0, 0, -1 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, -1 }), .color = properties.color, .normal = { 0, 0, -1 } });

    // back
    vertices.push_back(Vertex { .position = transform_point({ -1, -1, 1 }), .color = properties.color, .normal = { 0, 0, 1 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, 1 }), .color = properties.color, .normal = { 0, 0, 1 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, 1 }), .color = properties.color, .normal = { 0, 0, 1 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, -1, 1 }), .color = properties.color, .normal = { 0, 0, 1 } });
    vertices.push_back(Vertex { .position = transform_point({ -1, 1, 1 }), .color = properties.color, .normal = { 0, 0, 1 } });
    vertices.push_back(Vertex { .position = transform_point({ 1, 1, 1 }), .color = properties.color, .normal = { 0, 0, 1 } });
}

}
