#include <Essa/Geometry/Distance.hpp>
#include <Essa/Geometry/Line2D.hpp>
#include <Essa/Geometry/LineSegment2D.hpp>

#include <gtest/gtest.h>

TEST(DistanceTest, LinePoint2D) {
    auto line = Eg::Line2D::from_axbyc({ 4, 3, 3 });
    EXPECT_FLOAT_EQ(Eg::distance(line, { 1, 6 }), 5);
    EXPECT_FLOAT_EQ(Eg::distance(line, { -11, -3 }), 10);
    EXPECT_FLOAT_EQ(Eg::distance(line, { -3, 3 }), 0);
}

TEST(DistanceTest, LineSegmentPoint2D) {
    auto line = Eg::LineSegment2D({ -3, 3 }, { 4, -3 });
    EXPECT_FLOAT_EQ(Eg::distance(line, { 1, 6 }), 4.8809353009198);
    EXPECT_FLOAT_EQ(Eg::distance(line, { -7, 7 }), 5.6568542494924);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
