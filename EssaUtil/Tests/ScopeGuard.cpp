#include <EssaUtil/Testing.hpp>

#include <EssaUtil/ScopeGuard.hpp>

TEST_CASE(scope_guard) {
    int a = 5;
    {
        ScopeGuard guard { [&a] { a = 10; } };
    }
    EXPECT_EQ(a, 10);
    return {};
}
