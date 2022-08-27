#include "ObjLoader.hpp"

#include <EssaGUI/gui/Application.hpp>
#include <LLGL/Core/Vertex.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

namespace Essa {

std::optional<Model> ObjLoader::load_object_from_file(std::string const& filename) {
    std::ifstream file { filename };
    if (file.fail())
        return {};
    ObjLoader loader { file };
    return loader.load(std::filesystem::path { filename }.parent_path());
}

static inline void error(std::string_view message) {
    std::cerr << "ObjLoader: Failed to load OBJ file: " << message << std::endl;
}

std::optional<Model> ObjLoader::load(std::filesystem::path const& base_directory) {
    Model output;

    while (true) {
        std::string command;
        if (!(m_in >> command))
            break;

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
            m_positions.push_back({ x, y, z });
        }
        else if (command == "vt") {
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
        }
        else if (command == "vn") {
            float x, y, z;
            if (!(m_in >> x >> y >> z)) {
                error("invalid vn declaration");
                return {};
            }
            m_normals.push_back({ x, y, z });
        }
        else if (command == "f") {
            std::string line;
            if (!std::getline(m_in, line)) {
                error("failed to read f declaration");
                return {};
            }

            std::istringstream iss { line };
            std::vector<Vertex> face;
            while (!iss.eof()) {
                auto vertex = read_vertex(iss);
                if (!vertex)
                    return {};
                face.push_back(*vertex);
            }
            m_faces.push_back(face);
        }
        else if (command == "mtllib") {
            std::string path;
            if (!(m_in >> path)) {
                error("expected path after 'mtllib'");
                return {};
            }
            if (!load_mtl(base_directory / path, base_directory)) {
                return {};
            }
        }
        else if (command == "usemtl") {
            std::string name;
            if (!(m_in >> name)) {
                error("expected material name after 'usemtl'");
                return {};
            }
            output.set_material(m_materials[name]);
        }
        else {
            // std::cerr << "ObjLoader: Unknown command: " << command << std::endl;
            std::string line;
            std::getline(m_in, line);
        }
    }

    for (auto const& face : m_faces) {
        std::vector<llgl::Vertex> resolved_face;

        for (auto const& vertex : face) {
            if (vertex.position >= m_positions.size()) {
                error("vertex position out of range");
                return {};
            }
            auto position = m_positions[vertex.position];

            if (vertex.tex_coord && *vertex.tex_coord >= m_tex_coords.size()) {
                error("vertex tex coord out of range");
                return {};
            }
            auto tex_coord = vertex.tex_coord ? m_tex_coords[*vertex.tex_coord] : Util::Vector2f {};

            if (vertex.normal && *vertex.normal >= m_normals.size()) {
                error("vertex normal out of range");
                return {};
            }
            auto normal = vertex.normal ? m_normals[*vertex.normal] : Util::Vector3f {};

            resolved_face.push_back(llgl::Vertex { .position = position, .color = Util::Colors::White, .tex_coord = tex_coord, .normal = normal });
        }

        output.add_face(resolved_face);
    }
    return output;
}

std::optional<ObjLoader::Vertex> ObjLoader::read_vertex(std::istream& in) {
    size_t position = 0;
    if (!(in >> position)) {
        in.clear();
        error("invalid vertex index");
        return {};
    }
    if (in.get() != '/')
        return { { position - 1, {}, {} } };

    size_t tex_coord = 0;
    if (in.peek() != '/' && !(in >> tex_coord)) {
        error("invalid tex coord index");
        return {};
    }
    if (in.get() != '/')
        return { { position - 1, tex_coord == 0 ? std::optional<size_t> {} : tex_coord - 1, {} } };

    size_t normal;
    if (!(in >> normal)) {
        error("invalid normal index");
        return {};
    }
    return { { position - 1, tex_coord == 0 ? std::optional<size_t> {} : tex_coord - 1, normal - 1 } };
}

bool ObjLoader::load_mtl(std::string const& path, std::filesystem::path const& base_directory) {
    std::ifstream mtl_in { path };
    if (!mtl_in.good()) {
        error("mtl: failed to open file");
        return false;
    }

    std::optional<std::pair<std::string, Material>> current_material;

#define REQUIRE_CURRENT_MATERIAL()                 \
    *({                                            \
        if (!current_material) {                   \
            error("mtl: newmtl command required"); \
            return false;                          \
        }                                          \
        &current_material->second;                 \
    })

    while (true) {
        std::string command;
        if (!(mtl_in >> command)) {
            if (current_material)
                m_materials.insert(*current_material);
            return true;
        }

        if (command.starts_with("#")) {
            mtl_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (command == "newmtl") {
            if (current_material)
                m_materials.insert(*current_material);

            std::string name;
            if (!(mtl_in >> name)) {
                error("mtl: expected name after 'newmtl'");
                return false;
            }

            current_material = std::make_pair(name, Material {});
        }
        else if (command == "Ns"
            || command == "Ka"
            || command == "Ks"
            || command == "Ke"
            || command == "Ni"
            || command == "d"
            || command == "illum") {
            std::cout << "ObjLoader: Ignoring unimplemented mtl command: " << command << std::endl;
            mtl_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        else if (command == "Kd") {
            auto& material = REQUIRE_CURRENT_MATERIAL();
            float r;
            if (!(mtl_in >> r)) {
                error("mtl: failed to read r component in Kd");
                return false;
            }
            float g;
            if (!(mtl_in >> g)) {
                error("mtl: failed to read r component in Kd");
                return false;
            }
            float b;
            if (!(mtl_in >> b)) {
                error("mtl: failed to read r component in Kd");
                return false;
            }
            material.diffuse.color = Util::Colorf { r, g, b };
        }
        else if (command == "map_Kd") {
            auto& material = REQUIRE_CURRENT_MATERIAL();
            std::string path;
            if (!(mtl_in >> path)) {
                error("mtl: failed to read path in map_Kd");
                return false;
            }

            auto absolute_path = std::filesystem::absolute(std::filesystem::path { base_directory } / path);
            material.diffuse.texture = &GUI::Application::the().resource_manager().require_external<Gfx::Texture>(absolute_path.string());
        }
        else {
            // std::cerr << "ObjLoader: Unknown command: " << command << std::endl;
            std::string line;
            std::getline(m_in, line);
        }
    }
#undef REQUIRE_CURRENT_MATERIAL
}

}
