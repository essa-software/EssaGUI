#pragma once

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/OpenGL/Vertex.hpp>

namespace Gfx {

using LLGLVertex = llgl::Vertex<Util::Vector2f, Util::Colorf, Util::Vector2f>;

class Vertex : public llgl::Vertex<Util::Vector2f, Util::Colorf, Util::Vector2f> {
public:
    Vertex()
        : LLGLVertex { {}, {}, {} } { }

    Vertex(Util::Vector2f p, Util::Colorf c, Util::Vector2f t)
        : LLGLVertex(p, c, t) { }

    auto& position() { return value<0>(); }
    auto& position() const { return value<0>(); }
    auto& color() { return value<1>(); }
    auto& color() const { return value<1>(); }
    auto& tex_coord() { return value<2>(); }
    auto& tex_coord() const { return value<2>(); }
};

}
