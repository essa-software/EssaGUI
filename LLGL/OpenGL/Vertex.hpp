#pragma once

#include <GL/glew.h>

#include <EssaUtil/Color.hpp>
#include <EssaUtil/EnumBits.hpp>
#include <EssaUtil/Vector.hpp>
#include <GL/gl.h>

namespace llgl::opengl {

enum class PrimitiveType {
    Invalid = -1,
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Triangles = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
};

void begin(PrimitiveType);
void end();

class DrawScope {
public:
    DrawScope(PrimitiveType type) { begin(type); }
    ~DrawScope() { end(); }
};

void set_vertex_color(Util::Colorf);
void set_texture_coordinate(Util::Vector2f);
void add_vertex(Util::Vector3f);

}
