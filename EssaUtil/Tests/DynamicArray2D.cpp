#include <EssaUtil/Testing.hpp>

#include <EssaUtil/DynamicArray2D.hpp>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

TEST_CASE(constructors) {
    {
        // Default constructor
        Util::DynamicArray2D<int, 4, 4> arr { 2 };
        EXPECT_EQ(arr.get(0, 0), 2);
        EXPECT_EQ(arr.get(3, 0), 2);
        EXPECT_EQ(arr.get(0, 3), 2);
        EXPECT_EQ(arr.get(3, 3), 2);
    }
    {
        // Move constructor
        Util::DynamicArray2D<int, 4, 4> arr { 2 };
        auto arr2 = std::move(arr);
        // arr appears default-initialized
        EXPECT_EQ(arr.get(0, 0), 0);
        // arr2 contains data from arr
        EXPECT_EQ(arr2.get(0, 0), 2);
    }
    {
        // Move operator=
        Util::DynamicArray2D<int, 4, 4> arr { 2 };
        Util::DynamicArray2D<int, 4, 4> arr2;
        arr2 = std::move(arr);
        // arr appears default-initialized
        EXPECT_EQ(arr.get(0, 0), 0);
        // arr2 contains data from arr
        EXPECT_EQ(arr2.get(0, 0), 2);
        arr.set(0, 1, 2137);
        EXPECT_EQ(arr.get(0, 1), 2137);
    }
    return {};
}

TEST_CASE(ref) {
    Util::DynamicArray2D<int, 4, 4> arr { 2 };
    EXPECT_EQ(arr.get(0, 0), 2);
    arr.ref(2, 3) = 2137;
    EXPECT_EQ(arr.get(2, 3), 2137);
    return {};
}

TEST_CASE(set) {
    Util::DynamicArray2D<int, 4, 4> arr { 2 };
    EXPECT_EQ(arr.get(0, 0), 2);
    arr.set(2, 3, 2137);
    EXPECT_EQ(arr.get(2, 3), 2137);
    return {};
}

TEST_CASE(fill) {
    Util::DynamicArray2D<int, 4, 4> arr;
    arr.fill(2);
    EXPECT_EQ(arr.get(0, 0), 2);
    EXPECT_EQ(arr.get(3, 0), 2);
    EXPECT_EQ(arr.get(0, 3), 2);
    EXPECT_EQ(arr.get(3, 3), 2);
    return {};
}
