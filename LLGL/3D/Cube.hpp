#pragma once

#include <GL/glew.h>

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/Renderer/Renderable.hpp>
#include <vector>

namespace llgl {

class Cube : public Renderable {
public:
    explicit Cube();
    virtual void render(Renderer&, DrawState) const override;

private:
    void generate();

    opengl::VAO m_vao;
};

}
