#pragma once

#include <LLGL/Core/Vertex.hpp>
#include <LLGL/Renderer/Transform.hpp>
#include <vector>

namespace llgl::Shapes {

struct CubeProperties {
    Util::Vector3f position {};
    Util::Vector3f dimensions { 1, 1, 1 };
    Transform transform {}; // Is applied after position+dimensions
    Util::Color color = Util::Colors::White;
};

void add_cube(std::vector<Vertex>& vertices, CubeProperties properties = {});

}
