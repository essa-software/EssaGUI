#pragma once

#include <LLGL/OpenGL/Texture.hpp>
#include <string>

namespace Gfx {

llgl::opengl::Texture require_texture(std::string const& path);

}
