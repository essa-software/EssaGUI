#pragma once

#include <EssaUtil/Matrix.hpp>

namespace llgl
{

namespace opengl
{

class Shader;
class Texture;

}

struct DrawState
{
    opengl::Shader* shader = nullptr;
    opengl::Texture const* texture = nullptr;
    Util::Matrix4x4f model_matrix = Util::Matrix4x4f::identity();
    Util::Matrix4x4f view_matrix = Util::Matrix4x4f::identity();
};

}
