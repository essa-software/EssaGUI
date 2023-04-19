#include <EssaUtil/Testing.hpp>

#include <EssaUtil/CoordinateSystem.hpp>

using namespace Cs;

TEST_CASE(magnitude) {
    Cs::Vector3f test { 3, 4, 5 };
    EXPECT_EQ(test.length_squared(), 50);

    Cs::Vector3f zero;
    EXPECT_EQ(zero.length_squared(), 0);
    EXPECT_EQ(zero.length(), 0);
    EXPECT_EQ(zero.inverted_length(), INFINITY);

    return {};
}

TEST_CASE(length) {
    auto test = [](Cs::Vector3f v, float length) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v.length_squared(), length);
        return {};
    };
    TRY(test(Cs::Vector3f {}, 0));
    TRY(test(Cs::Vector3f { 1, 0, 0 }, 1));
    TRY(test(Cs::Vector3f { 0, 1, 0 }, 1));
    TRY(test(Cs::Vector3f { 0, 0, 1 }, 1));
    TRY(test(Cs::Vector3f { -1, 0, 0 }, 1));
    TRY(test(Cs::Vector3f { 1, 0, 1 }, 2));
    return {};
}

TEST_CASE(normalized) {
    auto test = [](Cs::Vector3f v, Cs::Vector3f n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.normalized().is_approximately_equal(n));
        return {};
    };
    TRY(test(Cs::Vector3f {}, Cs::Vector3f {}));
    TRY(test(Cs::Vector3f { 1, 0, 0 }, Cs::Vector3f { 1, 0, 0 }));
    TRY(test(Cs::Vector3f { 2, 0, 0 }, Cs::Vector3f { 1, 0, 0 }));
    TRY(test(Cs::Vector3f { 2, 2, 2 }, Cs::Vector3f { 0.57735026919, 0.57735026919, 0.57735026919 })); // 1/sqrt(3)
    return {};
}

TEST_CASE(dot) {
    auto test = [](float v, float n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v, n);
        return {};
    };
    TRY(test(Cs::Vector2f {}.dot(Cs::Vector2f {}), 0));
    TRY(test(Cs::Vector2f { 1, 1 }.dot(Cs::Vector2f {}), 0));
    TRY(test(Cs::Vector2f {}.dot(Cs::Vector2f { 1, 1 }), 0));
    TRY(test(Cs::Vector2f { 1, 1 }.dot(Cs::Vector2f { -1, 1 }), 0));
    TRY(test(Cs::Vector2f { -1, 1 }.dot(Cs::Vector2f { 1, 1 }), 0));
    TRY(test(Cs::Vector2f { 1, 1 }.dot(Cs::Vector2f { -2, 1 }), -1));
    TRY(test(Cs::Vector2f { 1, 1 }.dot(Cs::Vector2f { 0, 1 }), 1));
    return {};
}

TEST_CASE(rounding) {
    Cs::Vector3f test { 1.2, 3.5, -2.7 };

    Cs::Vector3f floor_result { 1, 3, -3 };
    Cs::Vector3f ceil_result { 2, 4, -2 };
    Cs::Vector3f round_result { 1, 4, -3 };

    EXPECT_EQ(test.floored(), floor_result);
    EXPECT_EQ(test.ceiled(), ceil_result);
    EXPECT_EQ(test.rounded(), round_result);

    return {};
}

TEST_CASE(with_length) {
    // This is mostly tested in normalized, so we don't need it here that much.
    auto test = [](Cs::Vector3f v, float l, Cs::Vector3f n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.with_length(l).is_approximately_equal(n));
        return {};
    };

    TRY(test(Cs::Vector3f {}, 0, Cs::Vector3f {}));
    TRY(test(Cs::Vector3f {}, 1, Cs::Vector3f {}));
    TRY(test(Cs::Vector3f { 1, -3, 0 }, 0, Cs::Vector3f {}));
    TRY(test(Cs::Vector3f { 1, -3, 0 }, 100, Cs::Vector3f { 31.62277660168, -94.86832980505, 0 }));
    return {};
}

TEST_CASE(clamp_length) {
    // This is mostly tested in normalized, so we don't need it here that much.
    auto test = [](Cs::Vector3f v, float l, Cs::Vector3f n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.clamp_length(l).is_approximately_equal(n));
        return {};
    };

    TRY(test(Cs::Vector3f {}, 0, Cs::Vector3f {}));
    TRY(test(Cs::Vector3f {}, 1, Cs::Vector3f {}));
    TRY(test(Cs::Vector3f { 1, -3, 0 }, 0, Cs::Vector3f {}));
    TRY(test(Cs::Vector3f { 1, -3, 0 }, 100, Cs::Vector3f { 1, -3, 0 }));
    TRY(test(Cs::Vector3f { 10000, -30000, 0 }, 100, Cs::Vector3f { 31.62277660168, -94.86832980505, 0 }));
    return {};
}

TEST_CASE(arithmetic) {
    Cs::Vector3f v1 { 5, 4, 2 };
    Cs::Vector3f v2 { -1, 4, 6 };
    Cs::Vector3f v1_plus_v2 { 4, 8, 8 };
    EXPECT_EQ(v1 + v2, v1_plus_v2);
    Cs::Vector3f v1_minus_v2 { 6, 0, -4 };
    EXPECT_EQ(v1 - v2, v1_minus_v2);

    Cs::Vector3f v1_multipled { 25, 20, 10 };
    Cs::Vector3f v1_divided { 1, 0.8, 0.4 };
    EXPECT_EQ(v1 * 5, v1_multipled);
    EXPECT_EQ(v1 / 5, v1_divided);

    return {};
}

TEST_CASE(create_polar) {
    auto test = [](Cs::Vector2f v, Cs::Vector2f n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.is_approximately_equal(n));
        return {};
    };
    TRY(test(Cs::Vector2f::create_polar(0_deg, 1), Cs::Vector2f { 1, 0 }));
    TRY(test(Cs::Vector2f::create_polar(90_deg, 1), Cs::Vector2f { 0, 1 }));
    TRY(test(Cs::Vector2f::create_polar(180_deg, 2), Cs::Vector2f { -2, 0 }));
    TRY(test(Cs::Vector2f::create_polar(270_deg, 2), Cs::Vector2f { 0, -2 }));
    return {};
}

TEST_CASE(angle) {
    auto test = [](Cs::Vector2f v, float n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v.angle().deg(), n);
        return {};
    };
    TRY(test(Cs::Vector2f {}, 0));
    TRY(test(Cs::Vector2f { 1, 0 }, 0));
    TRY(test(Cs::Vector2f { 0, 1 }, 90));
    TRY(test(Cs::Vector2f { -2, 0.001 }, 179.97134));
    TRY(test(Cs::Vector2f { -2, 0 }, 180));
    TRY(test(Cs::Vector2f { -2, -0.001 }, -179.97134));
    TRY(test(Cs::Vector2f { 0, -2 }, -90));
    return {};
}

TEST_CASE(directed_angle_to) {
    auto test = [](Cs::Vector2f v1, Cs::Vector2f v2, float a) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v1.directed_angle_to(v2).deg(), a);
        return {};
    };
    TRY(test(Cs::Vector2f { 0, 1 }, Cs::Vector2f { 1, 0 }, -90));
    TRY(test(Cs::Vector2f { 1, 0 }, Cs::Vector2f { 0, 1 }, 90));
    TRY(test(Cs::Vector2f { 1, 0 }, Cs::Vector2f { -1, 0 }, -180));
    TRY(test(Cs::Vector2f { -1, 0 }, Cs::Vector2f { 1, 0 }, 180));
    TRY(test(Cs::Vector2f { 0, -1 }, Cs::Vector2f { 1, -1 }, 45));
    TRY(test(Cs::Vector2f { -2, 0.001 }, Cs::Vector2f { -2, -0.001 }, 0.05730893));
    TRY(test(Cs::Vector2f { -2, -0.001 }, Cs::Vector2f { -2, 0.001 }, -0.05730893));
    return {};
}

TEST_CASE(rotate_2d) {
    auto test = [](Cs::Vector2f v, float a, Cs::Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.rotate(Angle::degrees(a)).is_approximately_equal(result));
        return {};
    };
    TRY(test(Cs::Vector2f {}, 90, Cs::Vector2f {}));
    TRY(test(Cs::Vector2f { 1, 0 }, 180, Cs::Vector2f { -1, 0 }));
    TRY(test(Cs::Vector2f { 0, 1 }, 180, Cs::Vector2f { 0, -1 }));
    TRY(test(Cs::Vector2f { 2, 0 }, 180, Cs::Vector2f { -2, 0 }));
    TRY(test(Cs::Vector2f { 0, 2 }, 180, Cs::Vector2f { 0, -2 }));
    TRY(test(Cs::Vector2f { M_SQRT1_2, M_SQRT1_2 }, 45, Cs::Vector2f { 0, 1 }));
    TRY(test(Cs::Vector2f { M_SQRT1_2, M_SQRT1_2 }, -45, Cs::Vector2f { 1, 0 }));
    TRY(test(Cs::Vector2f { M_SQRT1_2, M_SQRT1_2 }, 135, Cs::Vector2f { -1, 0 }));
    TRY(test(Cs::Vector2f { M_SQRT1_2, M_SQRT1_2 }, -135, Cs::Vector2f { 0, -1 }));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(perpendicular) {
    auto test = [](Cs::Vector2f v, Cs::Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.perpendicular().is_approximately_equal(result));
        return {};
    };
    TRY(test(Cs::Vector2f { 0, 1 }, Cs::Vector2f { -1, 0 }));
    TRY(test(Cs::Vector2f { 1, 0 }, Cs::Vector2f { 0, 1 }));
    TRY(test(Cs::Vector2f { 1, 1 }, Cs::Vector2f { -1, 1 }));
    TRY(test(Cs::Vector2f { -1, -1 }, Cs::Vector2f { 1, -1 }));
    return {};
}

TEST_CASE(mirror_against_tangent) {
    auto test = [](Cs::Vector2f v, Cs::Vector2f axis, Cs::Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.mirror_against_tangent(axis.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Cs::Vector2f { -1, 1 }, Cs::Vector2f { 1, 0 }, Cs::Vector2f { 1, 1 }));
    TRY(test(Cs::Vector2f { 1, 0 }, Cs::Vector2f { 1, 1 }, Cs::Vector2f { 0, -1 }));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(reflect_against_tangent) {
    auto test = [](Cs::Vector2f v, Cs::Vector2f axis, Cs::Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.reflect_against_tangent(axis.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Cs::Vector2f { 0, 1 }, Cs::Vector2f { 1, 0 }, Cs::Vector2f { 0, -1 }));
    TRY(test(Cs::Vector2f { 1, 1 }, Cs::Vector2f { 0, 1 }, Cs::Vector2f { -1, 1 }));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(tangent_part) {
    auto test = [](Cs::Vector2f v, Cs::Vector2f normal, Cs::Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.tangent_part(normal.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Cs::Vector2f { 1, -1 }, Cs::Vector2f { 0, 1 }, Cs::Vector2f { 0, -1 }));
    TRY(test(Cs::Vector2f { -1, 1 }, Cs::Vector2f { 1, 1 }, Cs::Vector2f {}));
    TRY(test(Cs::Vector2f { -1, 1 }, Cs::Vector2f { -1, -1 }, Cs::Vector2f {}));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(normal_part) {
    auto test = [](Cs::Vector2f v, Cs::Vector2f normal, Cs::Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.normal_part(normal.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Cs::Vector2f { 1, -1 }, Cs::Vector2f { 0, 1 }, Cs::Vector2f { 1, 0 }));
    TRY(test(Cs::Vector2f { -1, 1 }, Cs::Vector2f { 1, 1 }, Cs::Vector2f { -1, 1 }));
    TRY(test(Cs::Vector2f { -1, 1 }, Cs::Vector2f { -1, -1 }, Cs::Vector2f { -1, 1 }));
    // FIXME: Add more interesting cases
    return {};
}

// TODO: Add tests for:
// - Point
// - Size
// - Interoperability
