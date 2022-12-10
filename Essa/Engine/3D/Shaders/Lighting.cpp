#include "Lighting.hpp"

#include <Essa/Engine/3D/ShaderResource.hpp>
#include <Essa/GUI/Application.hpp>

namespace Essa::Shaders {

std::string_view Lighting::source(llgl::ShaderType type) const {
    return GUI::Application::the().resource_manager().require<FullShaderResource>("Lighting.shader").source(type);
}

}
