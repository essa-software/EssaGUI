#pragma once

#include "PrimitiveType.hpp"
#include "Shader.hpp"

namespace llgl {

template<ShaderImpl Shader> struct DrawState {
public:
    DrawState(Shader& shader, typename Shader::Uniforms uniforms, llgl::PrimitiveType pt)
        : m_shader(shader)
        , m_uniforms(std::move(uniforms))
        , m_primitive_type(pt) { }

    void apply() const { Detail::bind_shader(m_shader, m_uniforms); }

    llgl::PrimitiveType primitive_type() const { return m_primitive_type; }

private:
    Shader& m_shader;
    typename Shader::Uniforms m_uniforms;
    llgl::PrimitiveType m_primitive_type;
};

}
