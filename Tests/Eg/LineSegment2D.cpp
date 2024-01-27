#include <Essa/Geometry/LineSegment2D.hpp>

#include <gtest/gtest.h>

TEST(LineSegment2DTest, ConstructionAndBasicParams) {
    // Length
    EXPECT_EQ(Eg::LineSegment2D({ 0, 0 }, { 1, 1 }).length_squared(), 2);
    EXPECT_FLOAT_EQ(Eg::LineSegment2D({ 0, 0 }, { 1, 1 }).length(), std::sqrt(2));

    // Midpoint
    EXPECT_EQ(Eg::LineSegment2D({ 0, 0 }, { 2, 2 }).midpoint(), Util::Point2f(1, 1));
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
