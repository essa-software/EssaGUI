#include "Model.hpp"

#include "ObjLoader.hpp"

#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <EssaUtil/Vector.hpp>
#include <iostream>

std::optional<Essa::Model> Gfx::ResourceTraits<Essa::Model>::load_from_file(std::string const& path) {
    return Essa::ObjLoader::load_object_from_file(path);
}
