#include <Essa/Geometry/Intersection.hpp>
#include <Essa/Geometry/Line2D.hpp>

#include <gtest/gtest.h>

TEST(DistanceTest, LineLine2D) {
    {
        auto line1 = Eg::Line2D::from_axbyc({ 4, 3, 3 });
        auto line2 = Eg::Line2D::from_axbyc({ 2, 5, -9 });
        auto inter = Eg::intersection(line1, line2);
        EXPECT_TRUE(inter.is_point());
        EXPECT_TRUE(inter.point().is_approximately_equal({ -3, 3 }));
    }
    {
        auto line1 = Eg::Line2D::from_axbyc({ 4, 3, 3 });
        auto line2 = Eg::Line2D::from_axbyc({ -8, -6, -6 });
        EXPECT_TRUE(Eg::intersection(line1, line2) == Eg::Line2DIntersection::Overlapping);
    }

    {
        auto line1 = Eg::Line2D::from_axbyc({ 4, 3, 3 });
        auto line2 = Eg::Line2D::from_axbyc({ 4, 3, 5 });
        EXPECT_TRUE(Eg::intersection(line1, line2) == Eg::Line2DIntersection::Distinct);
    }
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
