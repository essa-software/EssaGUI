#pragma once

#include "Model.hpp"
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <Essa/LLGL/OpenGL/OpenGL.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>

#include <vector>

namespace Essa {

class Cube {
public:
    explicit Cube();

    void render(llgl::Renderer& renderer, llgl::ShaderImpl auto& shader) const {
        renderer.draw_vertices(m_vao, llgl::DrawState { shader, llgl::PrimitiveType::Triangles });
    }

private:
    void generate();

    llgl::VertexArray<Model::Vertex> m_vao;
};

}
