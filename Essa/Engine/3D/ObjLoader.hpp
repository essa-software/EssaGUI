#pragma once

#include "Material.hpp"
#include "Model.hpp"
#include <EssaUtil/CoordinateSystem.hpp>
#include <filesystem>
#include <istream>
#include <optional>

namespace Essa {

class ObjLoader {
public:
    ObjLoader(std::istream& in)
        : m_in(in) { }

    std::optional<Model> load(std::filesystem::path const& base_directory);

    static std::optional<Model> load_object_from_file(std::string const& filename);

private:
    struct Vertex {
        size_t position;
        std::optional<size_t> tex_coord;
        std::optional<size_t> normal;
    };

    std::optional<Vertex> read_vertex(std::istream& in);

    bool load_mtl(std::string const& path, std::filesystem::path const& base_directory);

    std::istream& m_in;
    std::vector<Util::Point3f> m_positions;
    std::vector<Util::Point2f> m_tex_coords;
    std::vector<Util::Vector3f> m_normals;

    std::map<std::string, Material> m_materials;
};

}
