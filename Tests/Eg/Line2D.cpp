#include <Essa/Geometry/Line2D.hpp>

#include <gtest/gtest.h>

TEST(Line2DTest, ConstructionAndBasicParams) {
    // From ax + by + c = 0
    {
        auto line = Eg::Line2D::from_axbyc({ 2, 3, 1 });
        EXPECT_FALSE(line.is_horizontal());
        EXPECT_FALSE(line.is_vertical());
        EXPECT_FLOAT_EQ(line.x_intercept(), -1 / 2.f);
        EXPECT_FLOAT_EQ(line.dydx(), -2 / 3.f);
        EXPECT_FLOAT_EQ(line.y_intercept(), -1 / 3.f);
        EXPECT_FLOAT_EQ(line.dxdy(), -3 / 2.f);
    }

    // From ax + b = y
    {
        auto line = Eg::Line2D::from_axb({ 2, 4 });
        EXPECT_FALSE(line.is_horizontal());
        EXPECT_FALSE(line.is_vertical());
        EXPECT_FLOAT_EQ(line.x_intercept(), -2);
        EXPECT_FLOAT_EQ(line.dydx(), 2);
        EXPECT_FLOAT_EQ(line.y_intercept(), 4);
        EXPECT_FLOAT_EQ(line.dxdy(), 1 / 2.f);
    }

    // Vertical
    {
        auto line = Eg::Line2D::vertical(5);
        EXPECT_TRUE(line.is_vertical());
        EXPECT_FLOAT_EQ(line.dxdy(), 0);
        EXPECT_FLOAT_EQ(line.x_intercept(), 5);
    }

    // Horizontal
    {
        auto line = Eg::Line2D::horizontal(5);
        EXPECT_TRUE(line.is_horizontal());
        EXPECT_FLOAT_EQ(line.dydx(), 0);
        EXPECT_FLOAT_EQ(line.y_intercept(), 5);
    }

    // From points
    {
        auto line = Eg::Line2D::from_points({ -4, 1 }, { 6, -3 });
        EXPECT_FLOAT_EQ(line.dydx(), -0.4);
        EXPECT_FLOAT_EQ(line.y_intercept(), -0.6);
    }

    // From points vertical
    {
        auto line = Eg::Line2D::from_points({ -4, 1 }, { -4, -2 });
        EXPECT_TRUE(line.is_vertical());
        EXPECT_FLOAT_EQ(line.x_intercept(), -4);
    }
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
