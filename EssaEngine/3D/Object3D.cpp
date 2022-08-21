#include "Object3D.hpp"

#include <EssaUtil/Vector.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/Renderer/Renderer.hpp>
#include <iostream>

#define OBJECT3D_DEBUG 0

namespace Essa {

void Object3D::add_face(Face face) {
    if constexpr (OBJECT3D_DEBUG) {
        m_normal_vertexes.push_back(llgl::Vertex { .position = face.v1.position });
        m_normal_vertexes.push_back(llgl::Vertex { .position = face.v1.position + face.v1.normal });
        m_normal_vertexes.push_back(llgl::Vertex { .position = face.v2.position });
        m_normal_vertexes.push_back(llgl::Vertex { .position = face.v2.position + face.v2.normal });
        m_normal_vertexes.push_back(llgl::Vertex { .position = face.v3.position });
        m_normal_vertexes.push_back(llgl::Vertex { .position = face.v3.position + face.v3.normal });
    }
    m_vertexes.push_back(std::move(face.v1));
    m_vertexes.push_back(std::move(face.v2));
    m_vertexes.push_back(std::move(face.v3));
    m_needs_update = true;
}

void Object3D::add_face(std::span<llgl::Vertex> face) {
    if (face.size() < 3)
        return;
    for (size_t s = 0; s < face.size() - 2; s++) {
        m_vertexes.push_back(face[0]);
        m_vertexes.push_back(face[s + 1]);
        m_vertexes.push_back(face[s + 2]);

        if constexpr (OBJECT3D_DEBUG) {
            m_normal_vertexes.push_back(llgl::Vertex { .position = face[0].position });
            m_normal_vertexes.push_back(llgl::Vertex { .position = face[0].position + face[0].normal });
            m_normal_vertexes.push_back(llgl::Vertex { .position = face[s + 1].position });
            m_normal_vertexes.push_back(llgl::Vertex { .position = face[s + 1].position + face[s + 1].normal });
            m_normal_vertexes.push_back(llgl::Vertex { .position = face[s + 2].position });
            m_normal_vertexes.push_back(llgl::Vertex { .position = face[s + 2].position + face[s + 2].normal });
        }
    }
    for (auto& v : m_normal_vertexes)
        std::cout << v.position << std::endl;
    m_needs_update = true;
}

void Object3D::render(llgl::Renderer& renderer, llgl::DrawState state) const {
    if (m_needs_update) {
        std::cerr << "needs update!" << std::endl;
        m_vao.load_vertexes(m_vertexes);
        if constexpr (OBJECT3D_DEBUG)
            m_normals_vao.load_vertexes(m_normal_vertexes);
        m_needs_update = false;
    }

    renderer.draw_vao(m_vao, llgl::opengl::PrimitiveType::Triangles, state);

    if constexpr (OBJECT3D_DEBUG)
        renderer.draw_vao(m_normals_vao, llgl::opengl::PrimitiveType::Triangles, state);
}

}
