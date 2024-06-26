#pragma once

#include <Essa/Engine/3D/Material.hpp>
#include <Essa/Engine/3D/Model.hpp>
#include <Essa/GUI/Graphics/ShaderResource.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Texture.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Transform.hpp>

namespace Essa::Shaders {

// FIXME: Recall which lighting algorithm it uses.
class Lighting {
public:
    using Vertex = Model::Vertex;

    struct Uniforms
        : public llgl::ShaderBases::Transform
        , public llgl::ShaderBases::Texture {
    private:
        Util::Point3f m_light_position;
        Util::Colorf m_light_color = Util::Colors::White;
        Util::Colorf m_ambient_color = Util::Colors::White;
        Util::Colorf m_diffuse_color = Util::Colors::White;
        Util::Colorf m_emission_color = Util::Colors::White;

    public:
        Uniforms() {
            set_material(Material { .ambient = {}, .diffuse = { Util::Colors::White }, .emission = {} });
        }

        void set_light_position(Util::Point3f lp) {
            m_light_position = lp;
        }
        void set_light_color(Util::Colorf lc) {
            m_light_color = lc;
        }

        static inline auto mapping
            = llgl::make_uniform_mapping(
                  llgl::Uniform { "lightPosition", &Uniforms::m_light_position }, llgl::Uniform { "lightColor", &Uniforms::m_light_color },
                  llgl::Uniform { "ambientColor", &Uniforms::m_ambient_color },
                  llgl::Uniform { "diffuseColor", &Uniforms::m_diffuse_color },
                  llgl::Uniform { "emissionColor", &Uniforms::m_emission_color }
              )
            | llgl::ShaderBases::Transform::mapping | llgl::ShaderBases::Texture::mapping;

        void set_material(Essa::Material const& material) {
            if (material.diffuse.texture) {
                set_texture(material.diffuse.texture);
            }
            m_ambient_color = material.ambient.color;
            m_diffuse_color = material.diffuse.color;
            m_emission_color = material.emission.color;
        }
    };

    static Gfx::FullShaderResource<Lighting>& load(Gfx::ResourceManager const&);
};

}
