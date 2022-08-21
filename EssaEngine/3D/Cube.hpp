#pragma once

#include <GL/glew.h>

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/Renderer/Renderable.hpp>
#include <vector>

namespace Essa {

class Cube : public llgl::Renderable {
public:
    explicit Cube();
    virtual void render(llgl::Renderer&, llgl::DrawState) const override;

private:
    void generate();

    llgl::opengl::VAO m_vao;
};

}
