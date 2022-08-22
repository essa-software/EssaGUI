#pragma once

#include "Model.hpp"

#include <istream>
#include <optional>

namespace Essa {

class ObjLoader {
public:
    ObjLoader(std::istream& in)
        : m_in(in) { }

    std::optional<Model> load();

    static std::optional<Model> load_object_from_file(std::string const& filename);

private:
    std::optional<llgl::Vertex> read_vertex(std::istream& in);

    std::istream& m_in;
    std::vector<Util::Vector3f> m_vertexes;
    std::vector<Util::Vector2f> m_tex_coords;
    std::vector<Util::Vector3f> m_normals;
};

}
