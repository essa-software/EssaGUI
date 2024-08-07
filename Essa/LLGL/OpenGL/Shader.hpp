#pragma once

#include <tuple>
#include "OpenGL.hpp"
#include "Texture.hpp"
#include <EssaUtil/Matrix.hpp>
#include <string_view>
#include <type_traits>

// Keep last!
#include <Essa/LLGL/Window/AbstractOpenGLHelper.hpp>

namespace llgl {

enum class ShaderType { Fragment, Vertex };

struct TextureUnit {
    int id = 0;
    llgl::Texture const* texture = nullptr;
};

namespace Detail {

// C++ -> OpenGL set uniform wrappers
inline void set_uniform(int location, TextureUnit const& tu) {
    OpenGL::ActiveTexture(GL_TEXTURE0 + tu.id);
    llgl::Texture::bind(tu.texture);
    OpenGL::Uniform1i(location, tu.id);
}

inline void set_uniform(int location, Util::Color const& color) {
    Util::Colorf colorf { color };
    OpenGL::Uniform4f(location, colorf.r, colorf.g, colorf.b, colorf.a);
}

inline void set_uniform(int location, Util::Matrix4x4f const& matrix) {
    OpenGL::UniformMatrix4fv(location, 1, true, matrix.raw_data());
}
inline void set_uniform(int location, Util::Point2f const& vec) {
    OpenGL::Uniform2f(location, vec.x(), vec.y());
}
inline void set_uniform(int location, Util::Point3f const& vec) {
    OpenGL::Uniform3f(location, vec.x(), vec.y(), vec.z());
}
inline void set_uniform(int location, Util::Vector2f const& vec) {
    OpenGL::Uniform2f(location, vec.x(), vec.y());
}
inline void set_uniform(int location, Util::Vector3f const& vec) {
    OpenGL::Uniform3f(location, vec.x(), vec.y(), vec.z());
}
inline void set_uniform(int location, Util::Size2f const& vec) {
    OpenGL::Uniform2f(location, vec.x(), vec.y());
}
inline void set_uniform(int location, Util::Size3f const& vec) {
    OpenGL::Uniform3f(location, vec.x(), vec.y(), vec.z());
}
inline void set_uniform(int location, int value) {
    OpenGL::Uniform1i(location, value);
}
inline void set_uniform(int location, bool value) {
    OpenGL::Uniform1i(location, value);
}
inline void set_uniform(int location, float value) {
    OpenGL::Uniform1f(location, value);
}

// Concat tuples
template<class T1, size_t... Id1, class T2, size_t... Id2>
decltype(auto) concat_tuples_impl(T1 const& t1, std::index_sequence<Id1...>, T2 const& t2, std::index_sequence<Id2...>) noexcept {
    return std::make_tuple(get<Id1>(t1)..., get<Id2>(t2)...);
}

template<class... T1, class... T2>
auto concat_tuples(std::tuple<T1...> const& t1, std::tuple<T2...> const& t2) {
    return concat_tuples_impl(t1, std::make_index_sequence<sizeof...(T1)>(), t2, std::make_index_sequence<sizeof...(T2)>());
}

} // namespace Detail

// ShaderImpl
template<class T>
concept ShaderImplPartial = requires(T t, T const ct, ShaderType st) {
    typename T::Vertex;
    typename T::Uniforms;
    { ct.source(st) } -> std::convertible_to<std::string_view>;
    { ct.program() } -> std::same_as<unsigned>;
    { t.set_program((unsigned)0) } -> std::same_as<void>;
    { T::Uniforms::mapping };
};

class Shader;

template<class MT>
struct Uniform {
    char const* name;
    MT member;
    int location = 0;

    Uniform(char const* name_, MT member_)
        : name(name_)
        , member(member_) { }
};

// Uniform Mapping - stores uniforms and their locations, responsible for binding them
template<class... Members>
class UniformMapping {
public:
    UniformMapping(Uniform<Members>&&... pairs)
        : m_uniforms(std::move(pairs)...) { }

    void bind(Shader& program, auto const& uniforms) {
        bind_impl(program, uniforms, std::make_index_sequence<sizeof...(Members)>());
    }

    template<class... Members2>
    auto operator|(UniformMapping<Members2...> const& other) const {
        return UniformMapping<Members..., Members2...>(Detail::concat_tuples(m_uniforms, other.m_uniforms));
    }
    template<class Member>
    auto operator|(Uniform<Member> const& u1) {
        return operator|(UniformMapping<Member>(u1));
    }

private:
    template<class... M2>
    friend class UniformMapping;

    explicit UniformMapping(std::tuple<Uniform<Members>...> pairs)
        : m_uniforms(std::move(pairs)) { }

    template<size_t... Idx>
    void bind_impl(Shader& shader, auto const& uniforms, std::index_sequence<Idx...>);

    template<class MT>
    int resolve_uniform_location(Shader& program, Uniform<MT>& member);

    std::tuple<Uniform<Members>...> m_uniforms;
};

template<class Member1, class Member2>
auto operator|(Uniform<Member1> u1, Uniform<Member2> u2) {
    return UniformMapping<Member1, Member2>(std::move(u1), std::move(u2));
}

template<class... Members>
UniformMapping<Members...> make_uniform_mapping(Uniform<Members>&&... members) {
    return { std::move(members)... };
}

template<class T>
concept ShaderImpl = std::is_base_of_v<Shader, T> && ShaderImplPartial<T>;

// Shader - RAII wrapper for OpenGL programs
class Shader {
public:
    ~Shader() {
        if (m_program)
            OpenGL::DeleteProgram(m_program);
    }

    unsigned program() const {
        return m_program;
    }
    void set_program(unsigned p) {
        m_program = p;
    }

private:
    unsigned m_program = 0;
};

template<class... Members>
template<size_t... Idx>
void UniformMapping<Members...>::bind_impl(Shader& shader, auto const& uniforms, std::index_sequence<Idx...>) {
    ((Detail::set_uniform(resolve_uniform_location(shader, std::get<Idx>(m_uniforms)), uniforms.*std::get<Idx>(m_uniforms).member)), ...);
}

template<class... Members>
template<class MT>
int UniformMapping<Members...>::resolve_uniform_location(Shader& program, Uniform<MT>& member) {
    if (!member.location) {
        member.location = OpenGL::GetUniformLocation(program.program(), member.name);
    }
    return member.location;
}

namespace Detail {

unsigned compile_shader(GLenum type, std::string_view source);
unsigned link_shader(std::initializer_list<unsigned> ids);

template<ShaderImplPartial Shader>
inline void bind_shader(Shader& shader, typename Shader::Uniforms const& uniforms) {
    if (!shader.program()) {
        auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, shader.source(ShaderType::Fragment));
        auto vertex_shader = compile_shader(GL_VERTEX_SHADER, shader.source(ShaderType::Vertex));
        shader.set_program(link_shader({ fragment_shader, vertex_shader }));
    }

    OpenGL::UseProgram(shader.program());
    using Uniforms = typename Shader::Uniforms;
    Uniforms::mapping.bind(shader, uniforms);
}

} // namespace Detail

} // namespace llgl
