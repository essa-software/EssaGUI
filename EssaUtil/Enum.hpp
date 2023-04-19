/*
 * Copyright (c) 2021, Brian Gianforcaro <bgianf@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Config.hpp"
#include <string_view>
#include <type_traits>

#define _ESSA_ENUM_DECL(Name, ...) \
    Name,

#define _ESSA_ENUM_DECL_WITH_VALUES(Name, Value, ...) \
    Name = Value,

#define _ESSA_ENUM_TO_STRING(Name, ...) \
    case _Enum::Name:                   \
        return #Name;

#define _ESSA_ENUM_FROM_STRING(Name, ...) \
    if (s == #Name)                       \
        return _Enum::Name;

// EnumMacro is expected to be of the form
// #define MY_ENUM_MACRO(K)
//    K(Name, [Value], <optional custom arguments...>)
//    K(Name, [Value], <optional custom arguments...>)
//    K(Name, [Value], <optional custom arguments...>)
//    K(Name, [Value], <optional custom arguments...>)
#define ESSA_ENUM(Enum, EnumMacro) \
    enum class Enum {              \
        EnumMacro(_ESSA_ENUM_DECL) \
    };

#define ESSA_ENUM_WITH_VALUES(Enum, EnumMacro) \
    enum class Enum {                          \
        EnumMacro(_ESSA_ENUM_DECL_WITH_VALUES) \
    };

#define ESSA_ENUM_TO_STRING(Enum, EnumMacro)              \
    constexpr inline std::string_view to_string(Enum e) { \
        using _Enum = Enum;                               \
        switch (e) {                                      \
            EnumMacro(_ESSA_ENUM_TO_STRING);              \
        }                                                 \
        ESSA_UNREACHABLE;                                 \
        return {};                                        \
    }

#define ESSA_ENUM_FROM_STRING(Enum, prefix, EnumMacro)                              \
    constexpr inline std::optional<Enum> prefix##_from_string(std::string_view s) { \
        using _Enum = Enum;                                                         \
        EnumMacro(_ESSA_ENUM_FROM_STRING);                                          \
        return {};                                                                  \
    }

#define ESSA_ENUM_FMT_FORMATTER(ToStringNamespace, Enum, EnumMacro)                         \
    namespace fmt {                                                                         \
    template<>                                                                              \
    struct formatter<ToStringNamespace::Enum> : public formatter<std::string_view> {        \
        template<typename FormatContext>                                                    \
        constexpr auto format(ToStringNamespace::Enum const& p, FormatContext& ctx) const { \
            return format_to(ctx.out(), "{}", ToStringNamespace::to_string(p));             \
        }                                                                                   \
    };                                                                                      \
    }

// Based on SerenityOS's AK:
// Enables bitwise operators for the specified Enum type.
//
#define ESSA_ENUM_BITWISE_OPERATORS(Enum) \
    _ESSA_ENUM_BITWISE_OPERATORS_INTERNAL(Enum, )

// Enables bitwise operators for the specified Enum type, this
// version is meant for use on enums which are private to the
// containing type.
//
#define ESSA_ENUM_BITWISE_FRIEND_OPERATORS(Enum) \
    _ESSA_ENUM_BITWISE_OPERATORS_INTERNAL(Enum, friend)

#define _ESSA_ENUM_BITWISE_OPERATORS_INTERNAL(Enum, Prefix)                \
                                                                           \
    [[nodiscard]] Prefix constexpr Enum operator|(Enum lhs, Enum rhs) {    \
        using Type = std::underlying_type_t<Enum>;                         \
        return static_cast<Enum>(                                          \
            static_cast<Type>(lhs) | static_cast<Type>(rhs));              \
    }                                                                      \
                                                                           \
    [[nodiscard]] Prefix constexpr Enum operator&(Enum lhs, Enum rhs) {    \
        using Type = std::underlying_type_t<Enum>;                         \
        return static_cast<Enum>(                                          \
            static_cast<Type>(lhs) & static_cast<Type>(rhs));              \
    }                                                                      \
                                                                           \
    [[nodiscard]] Prefix constexpr Enum operator^(Enum lhs, Enum rhs) {    \
        using Type = std::underlying_type_t<Enum>;                         \
        return static_cast<Enum>(                                          \
            static_cast<Type>(lhs) ^ static_cast<Type>(rhs));              \
    }                                                                      \
                                                                           \
    [[nodiscard]] Prefix constexpr Enum operator~(Enum rhs) {              \
        using Type = std::underlying_type_t<Enum>;                         \
        return static_cast<Enum>(                                          \
            ~static_cast<Type>(rhs));                                      \
    }                                                                      \
                                                                           \
    Prefix constexpr Enum& operator|=(Enum& lhs, Enum rhs) {               \
        using Type = std::underlying_type_t<Enum>;                         \
        lhs = static_cast<Enum>(                                           \
            static_cast<Type>(lhs) | static_cast<Type>(rhs));              \
        return lhs;                                                        \
    }                                                                      \
                                                                           \
    Prefix constexpr Enum& operator&=(Enum& lhs, Enum rhs) {               \
        using Type = std::underlying_type_t<Enum>;                         \
        lhs = static_cast<Enum>(                                           \
            static_cast<Type>(lhs) & static_cast<Type>(rhs));              \
        return lhs;                                                        \
    }                                                                      \
                                                                           \
    Prefix constexpr Enum& operator^=(Enum& lhs, Enum rhs) {               \
        using Type = std::underlying_type_t<Enum>;                         \
        lhs = static_cast<Enum>(                                           \
            static_cast<Type>(lhs) ^ static_cast<Type>(rhs));              \
        return lhs;                                                        \
    }                                                                      \
                                                                           \
    Prefix constexpr bool has_flag(Enum value, Enum mask) {                \
        using Type = std::underlying_type_t<Enum>;                         \
        return static_cast<Type>(value & mask) == static_cast<Type>(mask); \
    }                                                                      \
                                                                           \
    Prefix constexpr bool has_any_flag(Enum value, Enum mask) {            \
        using Type = std::underlying_type_t<Enum>;                         \
        return static_cast<Type>(value & mask) != 0;                       \
    }
