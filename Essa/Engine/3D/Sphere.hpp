#pragma once

#include "Model.hpp"
#include <Essa/LLGL/OpenGL/OpenGL.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <vector>

namespace Essa {

class Sphere {
public:
    explicit Sphere();

    void render(llgl::Renderer& renderer, llgl::ShaderImpl auto& shader, auto uniforms) const {
        renderer.draw_vertices(m_vao, llgl::DrawState { shader, uniforms, llgl::PrimitiveType::Triangles });
    }

private:
    void generate();
    size_t vertex_index(unsigned stack, unsigned sector) const;

    llgl::VertexArray<Model::Vertex> m_vao;
};

}
