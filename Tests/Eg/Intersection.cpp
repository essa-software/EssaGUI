#include <Essa/Geometry/Intersection.hpp>
#include <Essa/Geometry/Line2D.hpp>

#include <gtest/gtest.h>

namespace Eg {

void PrintTo(Line2DIntersection const& point, std::ostream* os) {
    if (point.is_point()) {
        *os << fmt::format("{}", point.point());
    }
    else {
        *os << (point == Line2DIntersection::Overlapping ? "Overlapping" : "Distinct");
    }
}

}

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

TEST(DistanceTest, LineLineSegment2D) {
    {
        auto ls1 = Eg::Line2D::from_points({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::LineSegment2D({ -3, 2 }, { 1, 4 });
        EXPECT_TRUE(Eg::intersection(ls1, ls2).point().is_approximately_equal(Util::Point2f(-2.6, 2.2)));
    }
    {
        auto ls1 = Eg::Line2D::from_points({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::LineSegment2D({ -1, 3 }, { 1, 4 });
        EXPECT_EQ(Eg::intersection(ls1, ls2), Eg::Line2DIntersection::Distinct);
    }
    {
        auto ls1 = Eg::LineSegment2D({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::Line2D::from_points({ -1, 3 }, { 1, 4 });
        EXPECT_TRUE(Eg::intersection(ls1, ls2).point().is_approximately_equal(Util::Point2f(-2.6, 2.2)));
    }
    {
        auto ls1 = Eg::LineSegment2D({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::Line2D::from_points({ 0, -3 }, { -1, -1 });
        EXPECT_EQ(Eg::intersection(ls1, ls2), Eg::Line2DIntersection::Overlapping);
    }
}

TEST(DistanceTest, LineSegment2D) {
    {
        auto ls1 = Eg::LineSegment2D({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::LineSegment2D({ -3, 2 }, { 1, 4 });
        EXPECT_TRUE(Eg::intersection(ls1, ls2).point().is_approximately_equal(Util::Point2f(-2.6, 2.2)));
    }
    {
        auto ls1 = Eg::LineSegment2D({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::LineSegment2D({ -1, 3 }, { 1, 4 });
        EXPECT_EQ(Eg::intersection(ls1, ls2), Eg::Line2DIntersection::Distinct);
    }
    {
        auto ls1 = Eg::LineSegment2D({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::LineSegment2D({ 0, -3 }, { -1, -1 });
        EXPECT_EQ(Eg::intersection(ls1, ls2), Eg::Line2DIntersection::Distinct);
    }
    {
        auto ls1 = Eg::LineSegment2D({ -1, -1 }, { 0, -3 });
        auto ls2 = Eg::LineSegment2D({ -3, 3 }, { -2, 1 });
        EXPECT_EQ(Eg::intersection(ls1, ls2), Eg::Line2DIntersection::Distinct);
    }
    {
        auto ls1 = Eg::LineSegment2D({ -3, 3 }, { -2, 1 });
        auto ls2 = Eg::LineSegment2D({ -3, 3 }, { -1, -1 });
        EXPECT_EQ(Eg::intersection(ls1, ls2), Eg::Line2DIntersection::Overlapping);
    }
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
