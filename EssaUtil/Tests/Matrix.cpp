#include <EssaUtil/Testing.hpp>

#include <EssaUtil/Matrix.hpp>

TEST_CASE(inverse) {
    {
        Util::Matrix4x4d matrix { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        EXPECT_EQ(matrix, matrix.inverted());
    }

    {
        Util::Matrix4x4d matrix { 5, 1, 3, 7, 3, -4, 5, 3, 3, 2, -2, 4, 6, 1, 7, 4 };
        Util::Matrix4x4d matrix_inverted { -172.0 / 325, 64.0 / 325,  7.0 / 13,  6.0 / 25, 51.0 / 325, -87.0 / 325, -2.0 / 13, 2.0 / 25,
                                           63.0 / 325,   -31.0 / 325, -4.0 / 13, 1.0 / 25, 27.0 / 65,  -4.0 / 65,   -3.0 / 13, -1.0 / 5 };
        EXPECT_EQ(matrix.inverted(), matrix_inverted);
    }
    return {};
}

TEST_CASE(multiply) {
    {
        Util::Matrix4x4d matrix1 { 5, 1, 3, 7, 3, -4, 5, 3, 3, 2, -2, 4, 6, 1, 7, 4 };
        Util::Matrix4x4d matrix2 { 1, -6, 7, 4, 2, 5, 2, -3, -1, 2, -1, 5, 6, 3, 7, 3 };
        Util::Matrix4x4d multiplied { 46, 2, 83, 53, 8, -19, 29, 58, 33, 0, 55, 8, 25, -5, 65, 68 };
        EXPECT_EQ(matrix1 * matrix2, multiplied);
    }
    return {};
}

BENCHMARK(multiply) {
    Util::Matrix4x4d matrix1 { 5, 1, 3, 7, 3, -4, 5, 3, 3, 2, -2, 4, 6, 1, 7, 4 };
    Util::Matrix4x4d matrix2 { 1, -6, 7, 4, 2, 5, 2, -3, -1, 2, -1, 5, 6, 3, 7, 3 };
    (void)(matrix1 * matrix2);
}
