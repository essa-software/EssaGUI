#pragma once

#include <cstdint>
#include <fmt/format.h>

#if __has_builtin(__builtin_source_location)
#    include <source_location>
#endif
namespace Util {

#if __has_builtin(__builtin_source_location)
using CppSourceLocation = std::source_location;
#else
// Yes. Dummy impl to satisfy clangD, again.
struct CppSourceLocation {
    static CppSourceLocation current() {
        return {};
    }
    constexpr uint_least32_t line() const noexcept {
        return 0;
    }
    constexpr uint_least32_t column() const noexcept {
        return 0;
    }
    constexpr char const* file_name() const noexcept {
        return "";
    }
    constexpr char const* function_name() const noexcept {
        return "";
    }
};

template<>
class fmt::formatter<Util::CppSourceLocation> : public fmt::formatter<std::string_view> {
public:
    static auto format(Util::CppSourceLocation const& p, fmt::format_context& ctx) {
        fmt::format_to(ctx.out(), "\e[33m{}()\e[m ({}:{}:{})", p.function_name(), p.file_name(), p.line(), p.column());
        return ctx.out();
    }
};
#endif

}
