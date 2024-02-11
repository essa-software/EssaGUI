#include <EssaUtil/Testing.hpp>

#include <EssaUtil/Buffer.hpp>

TEST_CASE(constructors) {
    // Default
    {
        Buffer buffer;
        EXPECT_EQ(buffer.size(), 0ull);
    }

    // std::span
    {
        uint8_t data[] { 0x12, 0x34, 0x56 };
        Buffer buffer { data };
        EXPECT_EQ(buffer.size(), 3ull);
        EXPECT_EQ(buffer[0], 0x12);
        EXPECT_EQ(buffer[1], 0x34);
        EXPECT_EQ(buffer[2], 0x56);
    }

    // Initializer list
    {
        Buffer buffer { 0x12, 0x34, 0x56 };
        EXPECT_EQ(buffer.size(), 3ull);
        EXPECT_EQ(buffer[0], 0x12);
        EXPECT_EQ(buffer[1], 0x34);
        EXPECT_EQ(buffer[2], 0x56);
    }

    // From std::string
    {
        Buffer buffer = Buffer::from_std_string("abc");
        EXPECT_EQ(buffer.size(), 3ull);
        EXPECT_EQ(buffer[0], 'a');
        EXPECT_EQ(buffer[1], 'b');
        EXPECT_EQ(buffer[2], 'c');
    }

    // Uninitialized
    {
        Buffer buffer = Buffer::uninitialized(40);
        EXPECT_EQ(buffer.size(), 40ull);
    }

    // Filled
    {
        Buffer buffer = Buffer::filled(40, 'a');
        EXPECT_EQ(buffer.size(), 40ull);
        EXPECT_EQ(buffer[0], 'a');
        EXPECT_EQ(buffer[39], 'a');
    }

    return {};
}

TEST_CASE(copy) {
    Buffer buffer1 = Buffer::filled(16, 'a');
    // Copy constructor
    {
        Buffer buffer2 { buffer1 };
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    // Copy assignment
    {
        Buffer buffer2;
        buffer2 = buffer1;
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    return {};
}

TEST_CASE(move) {
    // Move constructor
    {
        Buffer buffer1 = Buffer::filled(16, 'a');
        Buffer buffer2 { std::move(buffer1) };
        EXPECT_EQ(buffer1.size(), 0ull);
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    // Move assignment
    {
        Buffer buffer1 = Buffer::filled(16, 'a');
        Buffer buffer2;
        buffer2 = std::move(buffer1);
        EXPECT_EQ(buffer1.size(), 0ull);
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    return {};
}

TEST_CASE(clear) {
    Buffer buffer { 0x12, 0x34, 0x56 };
    buffer.clear();
    EXPECT_EQ(buffer.size(), (size_t)0);
    return {};
}

TEST_CASE(append) {
    Buffer buffer { 0x12, 0x34, 0x56 };
    buffer.append(0x78);
    EXPECT_EQ(buffer.size(), 4ull);
    EXPECT_EQ(buffer[3], 0x78);
    return {};
}

TEST_CASE(append_more) {
    Buffer buffer {};
    for (size_t s = 0; s < 1000; s++) {
        buffer.append(s & 0xff);
    }
    for (size_t s = 0; s < 1000; s++) {
        EXPECT_EQ(buffer[s], s & 0xff);
    }
    return {};
}

TEST_CASE(take_from_back) {
    Buffer buffer { 0x12, 0x34, 0x56, 0x78 };
    buffer.take_from_back(2);
    EXPECT_EQ(buffer[0], 0x12);
    EXPECT_EQ(buffer[1], 0x34);
    return {};
}

TEST_CASE(insert) {
    Buffer buffer { 0x12, 0x56, 0xbc };
    buffer.insert(1, 0x34);
    buffer.insert(3, { 0x78, 0x9a });
    uint8_t expected_result[] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc };
    EXPECT_EQ(buffer, Buffer { expected_result });
    return {};
}

TEST_CASE(resize_uninitialized) {
    // Grow
    {
        Buffer buffer { 0x12, 0x34 };
        buffer.resize_uninitialized(4);
        EXPECT_EQ(buffer.size(), 4ull);
        EXPECT_EQ(buffer[0], 0x12);
        EXPECT_EQ(buffer[1], 0x34);
    }

    // Shrink
    {
        Buffer buffer { 0x12, 0x34, 0x56, 0x78 };
        buffer.resize_uninitialized(2);
        Buffer expected { 0x12, 0x34 };
        EXPECT_EQ(buffer, expected);
    }
    return {};
}
