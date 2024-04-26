#pragma once

#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <filesystem>
#include <fmt/format.h>

namespace Gfx {

namespace Detail {
Util::OsErrorOr<std::string> read_file(std::string const& filename);
}

// source, program, set_program are handled by the resource wrapper
template<class T>
concept ResourceShaderImpl = requires(T t, T const ct, llgl::ShaderType st) {
    typename T::Vertex;
    typename T::Uniforms;
    { T::Uniforms::mapping };
};

/// The ShaderResource loads all shaders of given types from the following paths:
/// - <filename>.frag - fragment shader
/// - <filename>.vert - vertex shader
///
/// Example usage:
///
/// \code
/// struct MyShader : public llgl::Shader {
///     std::string_view source(llgl::ShaderType type) const {
///         // This will load from fragment and vertex shader from "assets/shaders/my_shader.frag"
///         // and "assets/shaders/my_shader.vert", respectively.
///         return GUI::Application::the().resource_manager()
///             .require<Essa::ShaderResource<llgl::ShaderType::Fragment, llgl::ShaderType::Vertex>>("my_shader");
///     }
/// };
/// \endcode
template<ResourceShaderImpl Shad, llgl::ShaderType... ShaderTypes>
class ShaderResource : public llgl::Shader {
public:
    using Vertex = typename Shad::Vertex;
    using Uniforms = typename Shad::Uniforms;

    std::string_view source(llgl::ShaderType st) const {
        auto it = m_sources.find(st);
        assert(it != m_sources.end());
        return it->second;
    }

private:
    friend struct Gfx::ResourceTraits<ShaderResource<Shad, ShaderTypes...>>;

    Shad m_shader;

    // FIXME: Let's do this the easy way first... (This should use something
    //        more lightweight than std::map)
    std::map<llgl::ShaderType, std::string> m_sources;
};

template<ResourceShaderImpl Shader>
using FullShaderResource = ShaderResource<Shader, llgl::ShaderType::Fragment, llgl::ShaderType::Vertex>;

}

template<Gfx::ResourceShaderImpl Shader, llgl::ShaderType... ShaderTypes>
struct Gfx::ResourceTraits<Gfx::ShaderResource<Shader, ShaderTypes...>> {
    static std::optional<Gfx::ShaderResource<Shader, ShaderTypes...>> load_from_file(std::string const& filename) {
        Gfx::ShaderResource<Shader, ShaderTypes...> resource;

        static std::map<llgl::ShaderType, std::string_view> s_shader_type_to_extension = {
            { llgl::ShaderType::Fragment, "frag" },
            { llgl::ShaderType::Vertex, "vert" },
        };

        std::filesystem::path filename_path = filename;

        bool error = false;
        auto load_source = [&]<llgl::ShaderType Type>() -> std::string {
            auto full_filename = filename_path.replace_extension(s_shader_type_to_extension[Type]).string();
            auto source_or_error = Gfx::Detail::read_file(full_filename);
            if (source_or_error.is_error()) {
                fmt::print("ShaderResource: Failed to load {}: {}\n", full_filename, source_or_error.error());
                error = true;
                return "";
            }
            return source_or_error.release_value();
        };

        ((resource.m_sources[ShaderTypes] = load_source.template operator()<ShaderTypes>()), ...);

        if (error) {
            return {};
        }
        return resource;
    }

    static std::string_view base_path() {
        return "shaders";
    }
};
