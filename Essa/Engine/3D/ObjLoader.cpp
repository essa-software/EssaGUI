#include "ObjLoader.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stddef.h>

namespace Essa {

DBG_DECLARE(Engine_ObjLoader_DumpUnimplementedMTLCommands);

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

    std::map<Material*, std::vector<std::vector<Vertex>>> faces_by_material;

    Material* current_material = nullptr;

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
            faces_by_material[current_material].push_back(std::move(face));
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
            auto material = m_materials.find(name);
            if (material == m_materials.end()) {
                current_material = nullptr;
            }
            else {
                current_material = &material->second;
            }
        }
        else {
            // std::cerr << "ObjLoader: Unknown command: " << command << std::endl;
            std::string line;
            std::getline(m_in, line);
        }
    }

    for (auto const& faces : faces_by_material) {
        std::vector<Model::Vertex> vertices;

        // Resolve vertex components
        for (auto const& face : faces.second) {
            std::vector<Model::Vertex> resolved_face;
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
                auto tex_coord = vertex.tex_coord ? m_tex_coords[*vertex.tex_coord] : Util::Point2f {};

                if (vertex.normal && *vertex.normal >= m_normals.size()) {
                    error("vertex normal out of range");
                    return {};
                }
                auto normal = vertex.normal ? m_normals[*vertex.normal] : Util::Vector3f {};

                resolved_face.push_back(Model::Vertex { position, Util::Colors::White, tex_coord, normal });
            }

            // Triangulate
            for (size_t s = 0; s < resolved_face.size() - 2; s++) {
                vertices.push_back(resolved_face[0]);
                vertices.push_back(resolved_face[s + 1]);
                vertices.push_back(resolved_face[s + 2]);
            }
        }

        output.add_range(vertices, faces.first ? *faces.first : std::optional<Material> {});
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

#define HANDLE_K_COMPONENT(name, target)                                                                                     \
    else if (command == "K" name) {                                                                                          \
        auto& material = REQUIRE_CURRENT_MATERIAL();                                                                         \
        float r;                                                                                                             \
        if (!(mtl_in >> r)) {                                                                                                \
            error("mtl: failed to read r component in K" name);                                                              \
            return false;                                                                                                    \
        }                                                                                                                    \
        float g;                                                                                                             \
        if (!(mtl_in >> g)) {                                                                                                \
            error("mtl: failed to read r component in K" name);                                                              \
            return false;                                                                                                    \
        }                                                                                                                    \
        float b;                                                                                                             \
        if (!(mtl_in >> b)) {                                                                                                \
            error("mtl: failed to read r component in K" name);                                                              \
            return false;                                                                                                    \
        }                                                                                                                    \
        target.color = Util::Colorf { r, g, b };                                                                             \
    }                                                                                                                        \
    else if (command == "map_K" name) {                                                                                      \
        auto& material = REQUIRE_CURRENT_MATERIAL();                                                                         \
        std::string path;                                                                                                    \
        if (!(mtl_in >> path)) {                                                                                             \
            error("mtl: failed to read path in map_K" name);                                                                 \
            return false;                                                                                                    \
        }                                                                                                                    \
        auto absolute_path = std::filesystem::absolute(std::filesystem::path { base_directory } / path);                     \
        target.texture = &GUI::Application::the().resource_manager().require_external<Gfx::Texture>(absolute_path.string()); \
    }

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
            || command == "Ks"
            || command == "Ni"
            || command == "d"
            || command == "illum") {
            if (DBG_ENABLED(Engine_ObjLoader_DumpUnimplementedMTLCommands)) {
                std::cout << "ObjLoader: Ignoring unimplemented mtl command: " << command << std::endl;
            }
            mtl_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        HANDLE_K_COMPONENT("a", material.ambient)
        HANDLE_K_COMPONENT("d", material.diffuse)
        HANDLE_K_COMPONENT("e", material.emission)
        else {
            // std::cerr << "ObjLoader: Unknown command: " << command << std::endl;
            std::string line;
            std::getline(m_in, line);
        }
    }
#undef REQUIRE_CURRENT_MATERIAL
}
}
