#pragma once

#include <Essa/LLGL/Core/Transform.hpp>
#include <EssaUtil/Color.hpp>
#include <vector>

#include "Essa/LLGL/OpenGL/MappedVertex.hpp"
#include "Essa/LLGL/OpenGL/Vertex.hpp"
#include "Model.hpp"

namespace Essa::Shapes {

struct CubeProperties {
    Util::Point3f position {};
    Util::Size3f dimensions { 1, 1, 1 };
    llgl::Transform transform {}; // Is applied after position+dimensions
    Util::Color color = Util::Colors::White;
};

void add_cube(std::vector<Model::Vertex>& vertices, CubeProperties properties = {});

template<llgl::MappableVertex Vertex>
void add_wireframe_cube(std::vector<Vertex>& vertices) {
    auto add = [&](Util::Point3f position) {
        Vertex vertex;
        llgl::MappedVertex<Vertex> mapping { vertex };
        mapping.set_position(position);
        mapping.set_color(Util::Colors::White);
        vertices.push_back(vertex);
    };

    add({ -1, -1, -1 });
    add({ 1, -1, -1 });
    add({ -1, 1, -1 });
    add({ 1, 1, -1 });
    add({ -1, -1, 1 });
    add({ 1, -1, 1 });
    add({ -1, 1, 1 });
    add({ 1, 1, 1 });

    add({ -1, -1, -1 });
    add({ -1, 1, -1 });
    add({ 1, -1, -1 });
    add({ 1, 1, -1 });
    add({ -1, -1, 1 });
    add({ -1, 1, 1 });
    add({ 1, -1, 1 });
    add({ 1, 1, 1 });

    add({ -1, -1, -1 });
    add({ -1, -1, 1 });
    add({ 1, -1, -1 });
    add({ 1, -1, 1 });
    add({ -1, 1, -1 });
    add({ -1, 1, 1 });
    add({ 1, 1, -1 });
    add({ 1, 1, 1 });
}

}
