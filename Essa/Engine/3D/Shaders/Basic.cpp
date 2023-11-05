#include "Basic.hpp"

#include <EssaUtil/Config.hpp>

namespace Essa::Shaders {

Gfx::FullShaderResource<Basic> Basic::load(Gfx::ResourceManager& manager) {
    return manager.require<Gfx::FullShaderResource<Basic>>("Basic.shader");
}

}
