#include "Vertex.hpp"

#include "Error.hpp"

#include <GL/gl.h>

namespace llgl::opengl
{

void begin(PrimitiveType type)
{
    glBegin(static_cast<GLenum>(type));
}

void end()
{
    ErrorHandler handler;
    glEnd();
}

void set_vertex_color(Util::Colorf color)
{
    glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}

void set_texture_coordinate(Util::Vector2f tex_coord)
{
    glTexCoord2f(tex_coord.x(), tex_coord.y());
}

void add_vertex(Util::Vector3f position)
{
    glVertex3f(position.x(), position.y(), position.z());
}

}
