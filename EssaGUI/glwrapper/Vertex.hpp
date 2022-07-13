#pragma once

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>

// TODO: Remove this when EssaGUI gets ported to LLGL
struct Vertex {
    Util::Vector3f position;
    Util::Colorf color {};
    Util::Vector2f tex_coord {};
    Util::Vector3f normal {};
};
