#pragma once

#include <GL/glew.h>

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/Renderer/Renderable.hpp>
#include <vector>

namespace Essa {

class Sphere : public llgl::Renderable {
public:
    explicit Sphere();
    virtual void render(llgl::Renderer&, llgl::DrawState) const override;

private:
    void generate();
    size_t vertex_index(unsigned stack, unsigned sector) const;

    llgl::opengl::VAO m_vao;
};

}
