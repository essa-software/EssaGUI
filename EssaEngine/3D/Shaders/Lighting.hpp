#pragma once

#include <EssaEngine/3D/Material.hpp>
#include <EssaEngine/3D/Model.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/ShaderBases/Texture.hpp>
#include <LLGL/OpenGL/ShaderBases/Transform.hpp>

namespace Essa::Shaders {

// FIXME: Recall which lighting algorithm it uses.
class Lighting : public llgl::Shader
    , public llgl::ShaderBases::Transform
    , public llgl::ShaderBases::Texture {
public:
    Lighting() {
        set_material(Material { .ambient = {}, .diffuse = { Util::Colors::White }, .emission = {} });
    }

    using Vertex = Model::Vertex;

    void set_light_position(Util::Vector3f lp) { m_light_position = lp; }
    void set_light_color(Util::Colorf lc) { m_light_color = lc; }

    auto uniforms() {
        return llgl::UniformList {
            m_light_position,
            m_light_color,
            m_ambient_color,
            m_diffuse_color,
            m_emission_color
        }
        + llgl::ShaderBases::Transform::uniforms()
            + llgl::ShaderBases::Texture::uniforms();
    }

    std::string_view source(llgl::ShaderType type) const;

    void set_material(Essa::Material const& material) {
        if (material.diffuse.texture)
            set_texture(material.diffuse.texture);
        m_ambient_color = material.ambient.color;
        m_diffuse_color = material.diffuse.color;
        m_emission_color = material.emission.color;
    }

private:
    llgl::Uniform<Util::Vector3f> m_light_position { "lightPosition" };
    llgl::Uniform<Util::Colorf> m_light_color { "lightColor" };

    llgl::Uniform<Util::Colorf> m_ambient_color { "ambientColor" };
    llgl::Uniform<Util::Colorf> m_diffuse_color { "diffuseColor" };
    llgl::Uniform<Util::Colorf> m_emission_color { "emissionColor" };
};

}
