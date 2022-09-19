#include "Model.hpp"

#include "ObjLoader.hpp"

#include <EssaUtil/Vector.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <iostream>

namespace Essa {

void Model::add_face(Face face) {
    if constexpr (OBJECT3D_DEBUG) {
        m_normal_vertexes.push_back({ face.v1.value<0>(), Util::Colors::White, {} });
        m_normal_vertexes.push_back({ face.v1.value<0>() + face.v1.value<3>(), Util::Colors::White, {} });
        m_normal_vertexes.push_back({ face.v2.value<0>(), Util::Colors::White, {} });
        m_normal_vertexes.push_back({ face.v2.value<0>() + face.v2.value<3>(), Util::Colors::White, {} });
        m_normal_vertexes.push_back({ face.v3.value<0>(), Util::Colors::White, {} });
        m_normal_vertexes.push_back({ face.v3.value<0>() + face.v3.value<3>(), Util::Colors::White, {} });
    }
    m_vertexes.push_back(std::move(face.v1));
    m_vertexes.push_back(std::move(face.v2));
    m_vertexes.push_back(std::move(face.v3));
    m_needs_update = true;
}

void Model::add_face(std::span<Vertex> face) {
    if (face.size() < 3)
        return;
    for (size_t s = 0; s < face.size() - 2; s++) {
        m_vertexes.push_back(face[0]);
        m_vertexes.push_back(face[s + 1]);
        m_vertexes.push_back(face[s + 2]);

        if constexpr (OBJECT3D_DEBUG) {
            m_normal_vertexes.push_back({ face[0].value<0>(), Util::Colors::White, {} });
            m_normal_vertexes.push_back({ face[0].value<0>() + face[0].value<3>(), Util::Colors::White, {} });
            m_normal_vertexes.push_back({ face[s + 1].value<0>(), Util::Colors::White, {} });
            m_normal_vertexes.push_back({ face[s + 1].value<0>() + face[s + 1].value<3>(), Util::Colors::White, {} });
            m_normal_vertexes.push_back({ face[s + 2].value<0>(), Util::Colors::White, {} });
            m_normal_vertexes.push_back({ face[s + 2].value<0>() + face[s + 2].value<3>(), Util::Colors::White, {} });
        }
    }
    m_needs_update = true;
}

}

std::optional<Essa::Model> Gfx::ResourceTraits<Essa::Model>::load_from_file(std::string const& path) {
    return Essa::ObjLoader::load_object_from_file(path);
}
