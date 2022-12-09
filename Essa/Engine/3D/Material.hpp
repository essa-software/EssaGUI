#pragma once

#include <EssaUtil/Color.hpp>
#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <string>

namespace Essa {

struct Material {
    struct Component {
        Util::Color color;
        llgl::Texture* texture = nullptr;
    };

    Component ambient;
    Component diffuse;
    Component emission;
};

}
