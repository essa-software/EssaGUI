#pragma once

#include <type_traits>
#include <utility>

#include "VertexMapping.hpp"

namespace llgl {

template<class T>
concept Empty = std::is_empty_v<T>;

template<class T>
concept MappableVertex = requires(T v) {
    v.template value<0>();
    { VertexMapping<T>() } -> Empty;
};

#define DEFINE_NAMED_ATTRIBUTE(snake_case, PascalCase)                                                                             \
    static constexpr bool Has##PascalCase = requires() { Mapping::snake_case; };                                                   \
                                                                                                                                   \
    auto snake_case() const                                                                                                        \
        requires Has                                                                                                               \
    ##PascalCase {                                                                                                                 \
        return m_vertex.template value<Mapping::snake_case>();                                                                     \
    }                                                                                                                              \
    void set_##snake_case(auto v)                                                                                                  \
        requires Has                                                                                                               \
    ##PascalCase {                                                                                                                 \
        m_vertex.template value<Mapping::snake_case>()                                                                             \
            = static_cast<std::remove_reference_t<decltype(std::declval<VertexType>().template value<Mapping::snake_case>())>>(v); \
    }

template<MappableVertex V>
class MappedVertex {
public:
    using VertexType = V;

    MappedVertex(VertexType& v)
        : m_vertex(v) { }

    using Mapping = VertexMapping<V>;

    DEFINE_NAMED_ATTRIBUTE(position, Position)
    DEFINE_NAMED_ATTRIBUTE(color, Color)
    DEFINE_NAMED_ATTRIBUTE(tex_coord, TexCoord)
    DEFINE_NAMED_ATTRIBUTE(normal, Normal)

private:
    V& m_vertex;
};

#undef DEFINE_NAMED_ATTRIBUTE

}
