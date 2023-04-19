#include <EssaUtil/Testing.hpp>

#include <EssaUtil/Stream.hpp>
#include <EssaUtil/Stream/File.hpp>
#include <EssaUtil/UString.hpp>
#include <vector>

ErrorOr<void, __TestSuite::TestError> expect_buffers_equal(std::span<uint8_t const> got, std::span<uint8_t const> expected) {
    EXPECT_EQ(got.size(), expected.size());
    EXPECT(std::equal(got.begin(), got.end(), expected.begin()));
    return {};
}

TEST_CASE(writable_memory_stream) {

    Util::WritableMemoryStream out;

    EXPECT_NO_ERROR(Writer { out }.write("testąę"));
    TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x74, 0x65, 0x73, 0x74, 0xc4, 0x85, 0xc4, 0x99 } }));
    EXPECT_NO_ERROR(Writer { out }.write("siema tej"));
    TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x74, 0x65, 0x73, 0x74, 0xc4, 0x85, 0xc4, 0x99, 0x73, 0x69, 0x65, 0x6d, 0x61, 0x20, 0x74, 0x65, 0x6a } }));

    return {};
}

TEST_CASE(writer_fmt_integration) {
    Util::WritableMemoryStream out;
    Writer writer { out };
    writer.writeff("{}", 12.25);
    TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { '1', '2', '.', '2', '5' } }));
    return {};
}

TEST_CASE(writer_endian) {
    {
        Util::WritableMemoryStream out;
        EXPECT_NO_ERROR(Writer { out }.write_little_endian<uint32_t>(0x12345678));
        TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x78, 0x56, 0x34, 0x12 } }));
        EXPECT_NO_ERROR(Writer { out }.write_big_endian<uint32_t>(0x12345678));
        TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x78, 0x56, 0x34, 0x12, 0x12, 0x34, 0x56, 0x78 } }));
    }

    // Float
    {
        Util::WritableMemoryStream out;
        EXPECT_NO_ERROR(Writer { out }.write_big_endian<float>(123.f));
        TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x42, 0xF6, 0x00, 0x00 } }));
    }
    {
        Util::WritableMemoryStream out;
        EXPECT_NO_ERROR(Writer { out }.write_little_endian<float>(123.f));
        TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x00, 0x00, 0xF6, 0x42 } }));
    }

    // Double
    {
        Util::WritableMemoryStream out;
        EXPECT_NO_ERROR(Writer { out }.write_big_endian<double>(123456.0));
        TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x40, 0xFE, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00 } }));
    }
    {
        Util::WritableMemoryStream out;
        EXPECT_NO_ERROR(Writer { out }.write_little_endian<double>(123456.0));
        TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xFE, 0x40 } }));
    }

    return {};
}

TEST_CASE(readable_memory_stream) {
    return {};

    uint8_t buffer[] { 0x65, 0x73, 0x73, 0x61, 0x67, 0x75, 0x69, 0x20, 0x69, 0x73, 0x20, 0x6f, 0x6b };

    Util::ReadableMemoryStream in { buffer };
    BinaryReader reader { in };

    {
        uint8_t read_buffer[6];
        EXPECT_NO_ERROR(reader.read(read_buffer));
        EXPECT_EQ(in.is_eof(), false);
        TRY(expect_buffers_equal(read_buffer, std::span<uint8_t const> { { 0x65, 0x73, 0x73, 0x61, 0x67, 0x75 } }));
    }

    {
        uint8_t read_buffer[20];
        EXPECT_EQ(reader.read_all(read_buffer).release_value(), 7);
        EXPECT_EQ(in.is_eof(), true);
        TRY(expect_buffers_equal(read_buffer, std::span<uint8_t const> { { 0x69, 0x20, 0x69, 0x73, 0x20, 0x6f, 0x6b } }));
    }

    return {};
}

TEST_CASE(binary_reader_buffering) {
    uint8_t big_buffer[16384];
    for (size_t s = 0; s < 16384; s++) {
        big_buffer[s] = rand() & 0xff;
    }

    {
        Util::ReadableMemoryStream in { big_buffer };
        Util::BinaryReader reader { in };

        uint8_t read_buffer[4000];
        EXPECT_NO_ERROR(reader.read(read_buffer));
        TRY(expect_buffers_equal(read_buffer, std::span { big_buffer, 4000 }));
        EXPECT_NO_ERROR(reader.read(read_buffer));
        TRY(expect_buffers_equal(read_buffer, std::span { big_buffer + 4000, 4000 }));
    }

    {
        Util::ReadableMemoryStream in { big_buffer };
        Util::BinaryReader reader { in };

        uint8_t read_buffer[8292]; // 2*BufferSize+100
        EXPECT_NO_ERROR(reader.read(read_buffer));
        TRY(expect_buffers_equal(read_buffer, std::span { big_buffer, 8292 }));
    }

    return {};
}

TEST_CASE(binary_reader_get_peek) {
    uint8_t buffer[] { 0x01, 0x02 };

    Util::ReadableMemoryStream in { buffer };
    Util::BinaryReader reader { in };

    uint8_t read_buffer[1];
    EXPECT_EQ(reader.peek().release_value().value(), 0x01);
    EXPECT_NO_ERROR(reader.read(read_buffer));
    EXPECT_EQ(read_buffer[0], 0x01);
    EXPECT_EQ(reader.peek().release_value().value(), 0x02);
    EXPECT_EQ(reader.get().release_value().value(), 0x02);
    EXPECT(!reader.peek().release_value().has_value());
    EXPECT(!reader.get().release_value().has_value());

    return {};
}

TEST_CASE(binary_reader_big_endian) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint8_t>().release_value(), 0x01ull);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint16_t>().release_value(), 0x0123ull);
    }

    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint32_t>().release_value(), 0x01234567ull);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint64_t>().release_value(), 0x0123456789abcdefull);
    }

    // Float
    {
        uint8_t buffer[] { 0x42, 0xF6, 0x00, 0x00 };
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_big_endian<float>().release_value(), 123.f);
    }

    // Double
    {
        uint8_t buffer[] { 0x40, 0xFE, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00 };
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_big_endian<double>().release_value(), 123456.0);
    }
    return {};
}

TEST_CASE(binary_reader_little_endian) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint8_t>().release_value(), 0x01ull);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint16_t>().release_value(), 0x2301ll);
    }

    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint32_t>().release_value(), 0x67452301ll);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint64_t>().release_value(), 0xefcdab8967452301ll);
    }

    // Float
    {
        uint8_t buffer[] { 0x00, 0x00, 0xF6, 0x42 };
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_little_endian<float>().release_value(), 123.f);
    }

    // Double
    {
        uint8_t buffer[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xFE, 0x40 };
        Util::ReadableMemoryStream in { buffer };
        Util::BinaryReader reader { in };
        EXPECT_EQ(reader.read_little_endian<double>().release_value(), 123456.0);
    }

    // TODO: Struct
    return {};
}

TEST_CASE(binary_reader_read_until) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
    Util::ReadableMemoryStream in { buffer };
    Util::BinaryReader reader { in };

    auto data_read = reader.read_until(0x67).release_value();
    Buffer expected { 0x01, 0x23, 0x45 };
    EXPECT_EQ(data_read, expected);
    EXPECT_EQ(reader.peek().release_value().value(), 0x89);

    return {};
}

TEST_CASE(binary_reader_read_while) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
    Util::ReadableMemoryStream in { buffer };
    Util::BinaryReader reader { in };

    auto data_read = reader.read_while([](uint8_t byte) { return byte < 0x67; }).release_value();
    Buffer expected { 0x01, 0x23, 0x45 };
    EXPECT_EQ(data_read, expected);
    EXPECT_EQ(reader.peek().release_value().value(), 0x67);

    return {};
}

TEST_CASE(text_reader_consume_while) {
    Util::ReadableMemoryStream in = Util::ReadableMemoryStream::from_string("test\nhello");
    Util::TextReader reader { in };

    EXPECT_EQ(reader.consume_while([](uint8_t c) { return c != '\n'; }).release_value().encode(), "test");
    EXPECT_NO_ERROR(reader.consume());
    EXPECT_EQ(reader.consume_while([](uint8_t c) { return c != 'o'; }).release_value().encode(), "hell");

    return {};
}

TEST_CASE(text_reader_consume_line) {
    Util::ReadableMemoryStream in = Util::ReadableMemoryStream::from_string("test\nhello");
    Util::TextReader reader { in };

    EXPECT_EQ(reader.consume_line().release_value().encode(), "test");
    EXPECT_EQ(reader.consume_line().release_value().encode(), "hello");

    return {};
}

TEST_CASE(file_streams) {
    constexpr char const* FileName = "/tmp/essautil-test";
    {
        // Write, Truncate
        auto writable_stream = Util::WritableFileStream::open(FileName, { .truncate = true }).release_value();
        uint8_t write_data[] = { 't', 'e', 's', 't' };
        EXPECT_NO_ERROR(Writer { writable_stream }.write_all(write_data));
    }

    {
        // Write, Append
        auto appending_stream = Util::WritableFileStream::open(FileName, {}).release_value();
        uint8_t write_data[] = { '1', '2', '3' };
        EXPECT_NO_ERROR(Writer { appending_stream }.write_all(write_data));
    }

    {
        // Write, fail if existing
        auto appending_stream = Util::WritableFileStream::open(FileName, { .fail_if_exists = true });
        EXPECT(appending_stream.is_error() && appending_stream.release_error().error == EEXIST);
    }

    {
        // Read
        auto readable_stream = Util::ReadableFileStream::open(FileName).release_value();
        uint8_t expected_data[] = { 't', 'e', 's', 't', '1', '2', '3' };
        uint8_t read_data[sizeof(expected_data)];
        EXPECT_NO_ERROR(BinaryReader { readable_stream }.read_all(read_data));
        TRY(expect_buffers_equal(expected_data, read_data));
    }

    remove(FileName);
    return {};
}

TEST_CASE(seek) {
    std::initializer_list<uint8_t> data = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    Util::ReadableMemoryStream out { data };

    Util::BinaryReader reader { out };

    EXPECT_EQ(reader.read_big_endian<uint32_t>().release_value(), 0x00112233ull);
    EXPECT_NO_ERROR(reader.seek(0, SeekDirection::FromStart));
    EXPECT_EQ(reader.read_big_endian<uint32_t>().release_value(), 0x00112233ull);
    EXPECT_NO_ERROR(reader.seek(4, SeekDirection::FromCurrent));
    EXPECT_EQ(reader.read_big_endian<uint32_t>().release_value(), 0x8899aabbull);
    EXPECT_NO_ERROR(reader.seek(-12, SeekDirection::FromEnd));
    EXPECT_EQ(reader.read_big_endian<uint32_t>().release_value(), 0x44556677ull);

    return {};
}
