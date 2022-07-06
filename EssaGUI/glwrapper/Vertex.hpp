#pragma once

#include <EssaUtil/Vector3.hpp>
#include "Color.hpp"

struct Vertex {
    Vector3 position;
    Color color;
    Vector3 tex_coords {};
};
