#pragma once

#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/VertexMapping.hpp>
#include <EssaUtil/Any.hpp>
#include <type_traits>

namespace llgl {

template<class V> class DynamicShader {
public:
    using Vertex = V;

    struct Uniforms {
        template<class T>
        Uniforms(T data)
            : data(std::move(data)) { }

        struct DynamicMapping {
            void bind(DynamicShader const& shader, Uniforms const& uniforms) { return shader.m_mapping_bind_func(uniforms); }
        };

        static inline DynamicMapping mapping;

        Util::Any data;
    };

    template<llgl::ShaderImplPartial T> DynamicShader(T& shader) {
        m_source_func = [&](llgl::ShaderType type) { return shader.source(type); };
        m_mapping_bind_func = [&](Uniforms const& uniforms) {
            using StrongUniforms = typename T::Uniforms;
            StrongUniforms::mapping.bind(shader, uniforms.data.template cast<StrongUniforms>());
        };
        m_program_func = [&]() { return shader.program(); };
        m_set_program_func = [&](unsigned id) { shader.set_program(id); };
    }

    std::string_view source(llgl::ShaderType type) const { return m_source_func(type); }
    unsigned program() const { return m_program_func(); }
    void set_program(unsigned id) { m_set_program_func(id); }

private:
    std::function<std::string_view(llgl::ShaderType)> m_source_func;
    std::function<void(Uniforms const&)> m_mapping_bind_func;
    std::function<unsigned()> m_program_func;
    std::function<void(unsigned)> m_set_program_func;
};

static_assert(llgl::ShaderImplPartial<DynamicShader<void>>);

}
