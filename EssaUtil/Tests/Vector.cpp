#include <EssaUtil/Testing.hpp>

#include <EssaUtil/Vector.hpp>

TEST_CASE(arithmetic) {
    Vector3f v1 { 5, 4, 2 };
    Vector3f v2 { -1, 4, 6 };
    Vector3f v1_plus_v2 { 4, 8, 8 };
    EXPECT_EQ(v1 + v2, v1_plus_v2);
    Vector3f v1_minus_v2 { 6, 0, -4 };
    EXPECT_EQ(v1 - v2, v1_minus_v2);

    Vector3f v1_multipled { 25, 20, 10 };
    Vector3f v1_divided { 1, 0.8, 0.4 };
    EXPECT_EQ(v1 * 5, v1_multipled);
    EXPECT_EQ(v1 / 5, v1_divided);

    return {};
}

TEST_CASE(magnitude) {
    Vector3f test { 3, 4, 5 };
    EXPECT_EQ(test.length_squared(), 50);

    Vector3f zero;
    EXPECT_EQ(zero.length_squared(), 0);
    EXPECT_EQ(zero.length(), 0);
    EXPECT_EQ(zero.inverted_length(), INFINITY);

    return {};
}

TEST_CASE(rounding) {
    Vector3f test { 1.2, 3.5, -2.7 };

    Vector3f floor_result { 1, 3, -3 };
    Vector3f ceil_result { 2, 4, -2 };
    Vector3f round_result { 1, 4, -3 };

    EXPECT_EQ(test.floored(), floor_result);
    EXPECT_EQ(test.ceiled(), ceil_result);
    EXPECT_EQ(test.rounded(), round_result);
    
    return {};
}

// TODO: angle, rotate, normalization,
