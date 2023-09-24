#include "EssaUtil/TemporaryChange.hpp"
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

TEST_CASE(temporary_change) {
    int a = 5;
    {
        TemporaryChange change { a, 10 };
        EXPECT_EQ(a, 10);
    }
    EXPECT_EQ(a, 5);
    return {};
}
