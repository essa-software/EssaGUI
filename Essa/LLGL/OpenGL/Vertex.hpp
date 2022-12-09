#pragma once

#include <cstddef>
#include <cstdint>
#include <fmt/ostream.h>
#include <tuple>
#include <type_traits>

namespace llgl {

template<class... Ts>
inline constexpr size_t SumOfSizeof = (sizeof(Ts) + ...);

template<size_t HowMuch, class T, class... Ts>
inline constexpr size_t SumOfSizeofSlice = sizeof(T) + SumOfSizeofSlice<HowMuch - 1, Ts...>;

template<class T, class... Ts>
inline constexpr size_t SumOfSizeofSlice<0, T, Ts...> = 0;

// https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
template<int N, typename... Ts>
using NthTypeOf =
    typename std::tuple_element<N, std::tuple<Ts...>>::type;

template<class... Args>
struct ParameterPack { };

template<class... Attributes>
requires(std::is_standard_layout_v<Attributes>&&...) class Vertex {
public:
    static constexpr size_t AttributeCount = sizeof...(Attributes);

    template<size_t Idx>
    using AttributeType = NthTypeOf<Idx, Attributes...>;

    using AttributePack = ParameterPack<Attributes...>;

    constexpr Vertex() {
        emplace_values<0>(Attributes {}...);
    }

    constexpr Vertex(Attributes... attrs) {
        emplace_values<0>(std::forward<Attributes>(attrs)...);
    }

    template<size_t Idx>
    requires(Idx < AttributeCount) static constexpr size_t offset() { return SumOfSizeofSlice<Idx, Attributes...>; }
    static constexpr size_t stride() { return SumOfSizeof<Attributes...>; }

    template<size_t Idx>
    requires(Idx < AttributeCount) constexpr auto& value() {
        return *reinterpret_cast<AttributeType<Idx>*>(&m_storage[offset<Idx>()]);
    }

    template<size_t Idx>
    requires(Idx < AttributeCount) constexpr auto& value() const {
        return *reinterpret_cast<AttributeType<Idx> const*>(&m_storage[offset<Idx>()]);
    }

private:
    template<size_t Offset>
    constexpr void emplace_value(auto attr) {
        new (m_storage + Offset) decltype(attr) { attr };
    }

    template<size_t Offset>
    constexpr void emplace_values(auto attr1) {
        emplace_value<Offset>(std::move(attr1));
    }

    template<size_t Offset, class... Args>
    constexpr void emplace_values(auto attr1, Args... attrs) {
        emplace_value<Offset>(std::move(attr1));
        emplace_values<Offset + sizeof(attr1)>(std::forward<Args>(attrs)...);
    }

    char m_storage[SumOfSizeof<Attributes...>] {};
};

template<class Lhs, class Rhs>
inline constexpr bool IsSameVertexLayout = std::is_same_v<typename Lhs::AttributePack, typename Rhs::AttributePack>;

}
