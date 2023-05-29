// No #pragma once, this is expected to be included only once per test file

#include <EssaUtil/Clock.hpp>
#include <EssaUtil/Error.hpp>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string_view>
#include <unistd.h>

using namespace Util;

template<class T> struct FormatIfFormattable {
    T const& t;
};

namespace fmt {
template<class T>
    requires(is_formattable<T>::value)
struct formatter<FormatIfFormattable<T>> : public formatter<T> {
    template<typename FormatContext> constexpr auto format(FormatIfFormattable<T> const& p, FormatContext& ctx) {
        return formatter<T>::format(p.t, ctx);
    }
};

template<class T> struct formatter<FormatIfFormattable<T>> : public formatter<void*> {
    template<typename FormatContext> constexpr auto format(FormatIfFormattable<T> const& p, FormatContext& ctx) const {
        return format_to(ctx.out(), "?{}@{:p}", typeid(p.t).name(), ptr(&p.t));
    }
};
}

namespace __TestSuite {

struct TestError {
    std::string expression;
    std::string_view file;
    int line;
    std::string cause = "";
};

ErrorOr<void, TestError> expect(bool condition, std::string_view expression, std::string_view file, int line) {
    if (!condition)
        return TestError { std::string { expression }, file, line, "condition is false" };
    return {};
}

ErrorOr<void, TestError> expect_equal(auto v1, auto v2, std::string_view expr1, std::string_view expr2, std::string_view file, int line) {
    if (v1 != v2) {
        return TestError { fmt::format("{} == {}", expr1, expr2), file, line,
                           fmt::format("'{}' != '{}'", FormatIfFormattable { v1 }, FormatIfFormattable { v2 }) };
    }
    return {};
}

ErrorOr<void, TestError>
expect_approx_equal(auto v1, auto v2, std::string_view expr1, std::string_view expr2, std::string_view file, int line) {
    if (std::abs(v1 - v2) > 10e-6) {
        return TestError { fmt::format("{} ~= {}", expr1, expr2), file, line,
                           fmt::format("'{}' !~= '{}'", FormatIfFormattable { v1 }, FormatIfFormattable { v2 }) };
    }
    return {};
}

template<class T, class E>
ErrorOr<void, TestError> expect_no_error(ErrorOr<T, E> value, std::string_view expr, std::string_view file, int line) {
    if (value.is_error()) {
        return TestError { fmt::format("!({}).is_error()", expr), file, line,
                           fmt::format("got error: {}", FormatIfFormattable { value.release_error() }) };
    }
    return {};
}

using Test = ErrorOr<void, TestError>();
using Benchmark = void();

std::map<std::string_view, Test*> tests;
std::map<std::string_view, Benchmark*> benchmarks;

}

#define EXPECT(...) TRY(__TestSuite::expect((__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__))
constexpr bool Fail = false;
#define FAIL() EXPECT(Fail)

#define EXPECT_EQ(e1, e2) TRY(__TestSuite::expect_equal((e1), (e2), #e1, #e2, __FILE__, __LINE__))
#define EXPECT_EQ_APPROX(e1, e2) TRY(__TestSuite::expect_approx_equal((e1), (e2), #e1, #e2, __FILE__, __LINE__))

#define EXPECT_NO_ERROR(...) TRY(__TestSuite::expect_no_error((__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__))

#define TEST_CASE(name)                                                                 \
    ErrorOr<void, __TestSuite::TestError> __test_##name##_func();                       \
    struct __Test_##name {                                                              \
        __Test_##name() { __TestSuite::tests.insert({ #name, __test_##name##_func }); } \
    } __test_##name##_adder;                                                            \
    ErrorOr<void, __TestSuite::TestError> __test_##name##_func()

#define BENCHMARK(name)                                                                                \
    void __benchmark_##name##_func();                                                                  \
    struct __Benchmark_##name {                                                                        \
        __Benchmark_##name() { __TestSuite::benchmarks.insert({ #name, __benchmark_##name##_func }); } \
    } __benchmark_##name##_adder;                                                                      \
    void __benchmark_##name##_func()

int main(int, char** argv) {
    bool failed = false;
    for (auto const& test : __TestSuite::tests) {
        std::string test_name = (std::string_view { argv[0] }.starts_with("./") ? std::string { argv[0] }.substr(2) : argv[0]) + "/"
            + std::string { test.first };
        std::cout << "\r\e[2K\e[33m . \e[m test: " << test_name << std::flush;
        auto result = test.second();
        if (result.is_error()) {
            auto error = result.release_error();

            std::cout << "\r\e[2K\e[31m ✗ \e[m" << test_name << ": expected \e[1m" << error.expression << "\e[m at \e[36m" << error.file
                      << ":" << error.line << "\e[m\n";
            if (!error.cause.empty()) {
                std::cout << " └─\e[m failed because \e[1m" << error.cause << "\e[m" << std::endl;
            }
            failed = true;
        }
    }
    std::cout << "\r\e[2K";
    for (auto const& benchmark : __TestSuite::benchmarks) {
        std::string test_name = (std::string_view { argv[0] }.starts_with("./") ? std::string { argv[0] }.substr(2) : argv[0]) + "/"
            + std::string { benchmark.first };
        std::cout << "\r\e[2K\e[33m . \e[m benchmark: " << test_name << std::flush;
        Util::Clock clock;
        using namespace std::chrono_literals;
        constexpr auto MaxRunTime = 10s;
        constexpr size_t MaxRuns = 1000000;
        size_t run_count = MaxRuns;
        for (size_t s = 0; s < MaxRuns; s++) {
            benchmark.second();
            if (clock.elapsed() > MaxRunTime) {
                run_count = s + 1;
                break;
            }
        }
        auto time = clock.elapsed();
        fmt::print(
            "\r\e\2K• \e[1m{}\e[m: {} run(s) finished in: {} ({} per test)\n", test_name, run_count, fmt::streamed(time),
            fmt::streamed(time / run_count)
        );
    }
    return failed ? 1 : 0;
}
