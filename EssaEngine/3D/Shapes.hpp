#pragma once

#include <LLGL/Core/Vertex.hpp>
#include <LLGL/Renderer/Transform.hpp>
#include <vector>

namespace Essa::Shapes {

struct CubeProperties {
    Util::Vector3f position {};
    Util::Vector3f dimensions { 1, 1, 1 };
    llgl::Transform transform {}; // Is applied after position+dimensions
    Util::Color color = Util::Colors::White;
};

void add_cube(std::vector<llgl::Vertex>& vertices, CubeProperties properties = {});

}
