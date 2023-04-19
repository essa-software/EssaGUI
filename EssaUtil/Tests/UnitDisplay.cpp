#include <EssaUtil/Testing.hpp>

#include <EssaUtil/UnitDisplay.hpp>

TEST_CASE(unit_display) {
    EXPECT_EQ(unit_display(1234, Quantity::Mass).to_string().encode(), "1.23 Tons");
    EXPECT_EQ(unit_display(10000000000, Quantity::Length).to_string().encode(), "1.0000 × 10^7 km");
    EXPECT_EQ(unit_display(1048576, Quantity::FileSize).to_string().encode(), "1 MiB");

    std::vector<Util::Unit> const resource_units {
        { "", 1 },
        { "K", 1e3 },
        { "M", 1e6 },
        { "G", 1e9 },
        { "T", 1e12 },
        { "P", 1e15 },
        { "E", 1e18 },
    };
    EXPECT_EQ(Util::unit_display(9990, resource_units, { .precision = 0 }).to_string().encode(), "10 K");
    EXPECT_EQ(Util::unit_display(100'000, resource_units, { .precision = 0 }).to_string().encode(), "100 K");

    return {};
}
