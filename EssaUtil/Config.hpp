#pragma once

#include "CppSourceLocation.hpp"
#include <tuple>
#include <type_traits>

#if defined(__clang__)
#    define ESSA_COMPILER_CLANG
#elif defined(__GNUC__)
#    define ESSA_COMPILER_GCC
#endif

#define ESSA_ALWAYS_INLINE [[gnu::always_inline]]

namespace Util {

[[noreturn]] void _crash(char const* message, CppSourceLocation const& = CppSourceLocation::current());

constexpr bool TODO = false;
#define ESSA_TODO Util::_crash("TODO")

constexpr bool UNREACHABLE = false;
#define ESSA_UNREACHABLE Util::_crash("Unreachable")

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct Overloaded : Ts... {
    Overloaded(Ts... ts)
        : Ts(std::forward<Ts>(ts))... { }
    using Ts::operator()...;
};

template<typename... Deps>
inline constexpr bool DependentFalse = false;

template<class T>
concept Enum = std::is_enum_v<T>;

namespace Detail {

template<class T, class... Ts>
struct _First {
    using Type = T;
};

}

template<class... Ts>
using First = typename Detail::_First<Ts...>::Type;

template<typename T, typename... Ts>
inline constexpr bool IsOneOf = (std::is_same_v<T, Ts> || ...);

// https://stackoverflow.com/questions/42580997/check-if-one-set-of-types-is-a-subset-of-the-other
template<typename Subset, typename Set>
inline constexpr bool IsSubsetOf = false;

template<typename... Ts, typename... Us>
inline constexpr bool IsSubsetOf<std::tuple<Ts...>, std::tuple<Us...>> = (IsOneOf<Ts, Us...> && ...);

template<class T>
concept Arithmetic = std::is_arithmetic_v<T>;

}
