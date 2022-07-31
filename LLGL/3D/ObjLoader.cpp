#include "ObjLoader.hpp"

#include <LLGL/Core/Vertex.hpp>

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

namespace llgl {

std::optional<Object3D> ObjLoader::load_object_from_file(std::string const& filename)
{
    std::ifstream file { filename };
    if (file.fail())
        return {};
    ObjLoader loader { file };
    return loader.load();
}

static inline void error(std::string_view message)
{
    std::cerr << "failed to load OBJ file: " << message << std::endl;
}

std::optional<Object3D> ObjLoader::load()
{
    Object3D output;

    while (true) {
        std::string command;
        if (!(m_in >> command))
            return output;

        if (command.starts_with("#")) {
            m_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        // std::cerr << command << std::endl;

        if (command == "v") {
            float x, y, z, w;
            if (!(m_in >> x >> y >> z)) {
                error("invalid v declaration");
                return {};
            }
            if (!(m_in >> w)) {
                m_in.clear();
                w = 1;
            }
            m_vertexes.push_back({ x, y, z });
        } else if (command == "vt") {
            float u, v, w;
            if (!(m_in >> u)) {
                error("invalid vt declaration");
                return {};
            }
            if (!(m_in >> v)) {
                m_in.clear();
                v = 0;
            }
            if (!(m_in >> w)) {
                m_in.clear();
                w = 0;
            }
            m_tex_coords.push_back({ u, v });
        } else if (command == "vn") {
            float x, y, z;
            if (!(m_in >> x >> y >> z)) {
                error("invalid vn declaration");
                return {};
            }
            m_normals.push_back({ x, y, z });
        } else if (command == "f") {
            std::vector<Vertex> face;
            std::string line;
            if (!std::getline(m_in, line)) {
                error("failed to read f declaration");
                return {};
            }
            std::istringstream iss { line };
            while (!iss.eof()) {
                auto vertex = read_vertex(iss);
                if (!vertex.has_value())
                    return {};
                face.push_back(vertex.value());
            }
            output.add_face(face);
        } else {
            // std::cerr << "ObjLoader: Unknown command: " << command << std::endl;
            std::string line;
            std::getline(m_in, line);
        }
    }
}

std::optional<Vertex> ObjLoader::read_vertex(std::istream& in)
{
    size_t vertex = 0, tex_coord = 0, normal = 0;
    if (!(in >> vertex) || vertex > m_vertexes.size()) {
        error("invalid vertex index");
        return {};
    }

    if (in.get() != '/')
        return Vertex { m_vertexes[vertex - 1], Util::Colors::White };
    if (!(in >> tex_coord) || tex_coord > m_tex_coords.size()) {
        error("invalid tex coord index");
        return {};
    }

    if (in.get() != '/')
        return Vertex { m_vertexes[vertex - 1], Util::Colors::White, m_tex_coords[tex_coord - 1] };
    if (!(in >> normal) || normal > m_normals.size()) {
        error("invalid normal index");
        return {};
    }
    // TODO: Store normals in Vertex
    return Vertex { m_vertexes[vertex - 1], Util::Colors::White, m_tex_coords[tex_coord - 1], m_normals[normal - 1] };
}

}
