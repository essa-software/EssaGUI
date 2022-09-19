#pragma once

#include "Shader.hpp"
#include "PrimitiveType.hpp"

namespace llgl {

template<ShaderImpl Shader>
struct DrawState {
public:
    DrawState(Shader& shader, llgl::PrimitiveType pt)
        : m_shader(shader)
        , m_primitive_type(pt) { }

    void apply() const {
        Detail::bind_shader(m_shader);
    }

    llgl::PrimitiveType primitive_type() const { return m_primitive_type; }

private:
    Shader& m_shader;
    llgl::PrimitiveType m_primitive_type;
};

}
