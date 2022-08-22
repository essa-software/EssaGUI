#pragma once

#include <EssaUtil/Color.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <string>

namespace Essa {

struct Material {
    struct Component {
        Util::Color color;
        llgl::opengl::Texture* texture = nullptr;
    };

    Component diffuse;
};

}
