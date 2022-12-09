#pragma once

#include <Essa/LLGL/Core/Transform.hpp>
#include <EssaUtil/Color.hpp>
#include <vector>

#include "Model.hpp"

namespace Essa::Shapes {

struct CubeProperties {
    Util::Vector3f position {};
    Util::Vector3f dimensions { 1, 1, 1 };
    llgl::Transform transform {}; // Is applied after position+dimensions
    Util::Color color = Util::Colors::White;
};

void add_cube(std::vector<Model::Vertex>& vertices, CubeProperties properties = {});

}
