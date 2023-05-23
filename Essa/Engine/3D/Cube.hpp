#pragma once

#include "Model.hpp"
#include <Essa/LLGL/OpenGL/OpenGL.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>

#include <vector>

namespace Essa {

class Cube {
public:
    enum class RenderingMode { Solid, Wireframe };
    explicit Cube(RenderingMode = RenderingMode::Solid);

    void render(llgl::Renderer& renderer, llgl::ShaderImpl auto& shader, auto uniforms) const {
        renderer.draw_vertices(
            m_vao,
            llgl::DrawState {
                shader,
                uniforms,
                m_rendering_mode == RenderingMode::Solid ? llgl::PrimitiveType::Triangles : llgl::PrimitiveType::Lines,
            }
        );
    }

private:
    void generate();

    llgl::VertexArray<Model::Vertex> m_vao;
    RenderingMode m_rendering_mode;
};

}
