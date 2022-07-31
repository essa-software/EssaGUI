#pragma once

#include "LLGL/OpenGL/Shader.hpp"
#include "LLGL/Renderer/Renderer.hpp"
#include <LLGL/Core/Vertex.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/Renderer/Renderable.hpp>
#include <LLGL/Renderer/Transform.hpp>
#include <vector>

namespace llgl
{

class Object3D : public Renderable
{
public:
    struct Face
    {
        Vertex v1, v2, v3;
    };

    void add_face(Face face);
    void add_face(std::span<Vertex>);

private:
    virtual void render(Renderer& renderer, DrawState) const override;

    std::vector<Vertex> m_vertexes;
    std::vector<Vertex> m_normal_vertexes;
    mutable opengl::VAO m_vao;
    mutable opengl::VAO m_normals_vao;
    mutable bool m_needs_update { true };
};

}
