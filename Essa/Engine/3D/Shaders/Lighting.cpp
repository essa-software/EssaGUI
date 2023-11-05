#include "Lighting.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/ShaderResource.hpp>

namespace Essa::Shaders {

Gfx::FullShaderResource<Lighting>& Lighting::load(Gfx::ResourceManager const& res) {
    return res.require<Gfx::FullShaderResource<Lighting>>("Lighting.shader");
}

}
