#include <Essa/Geometry/Distance.hpp>
#include <Essa/Geometry/Line2D.hpp>

#include <gtest/gtest.h>

TEST(DistanceTest, LinePoint2D) {
    auto line = Eg::Line2D::from_axbyc({ 4, 3, 3 });
    EXPECT_FLOAT_EQ(Eg::distance(line, { 1, 6 }), 5);
    EXPECT_FLOAT_EQ(Eg::distance(line, { -11, -3 }), 10);
    EXPECT_FLOAT_EQ(Eg::distance(line, { -3, 3 }), 0);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
