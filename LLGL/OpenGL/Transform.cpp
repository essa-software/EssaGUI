#include "Transform.hpp"

#include "Error.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

namespace llgl::opengl {

void set_viewport(Util::Recti rect)
{
    glViewport(rect.left, rect.top, rect.width, rect.height);
}

void set_matrix_mode(MatrixMode mode)
{
    glMatrixMode(static_cast<GLenum>(mode));
}

void load_identity()
{
    glLoadIdentity();
}

void load_ortho(OrthoArgs args)
{
    std::cout << "ortho " << args.left << "," << args.right << "," << args.bottom << "," << args.top << "," << args.near << "," << args.far << std::endl;
    glOrtho(args.left, args.right, args.bottom, args.top, args.near, args.far);
}

void load_perspective(PerspectiveArgs persp)
{
    std::cout << "perspective " << persp.fov << "," << persp.aspect << "," << persp.near << "," << persp.far << std::endl;
    gluPerspective(persp.fov, persp.aspect, persp.near, persp.far);
}

void push_matrix()
{
    glPushMatrix();
}

void pop_matrix()
{
    glPopMatrix();
}

}
