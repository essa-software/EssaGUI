#pragma once

#include "../Buffer.hpp"
#include "../Endianness.hpp"
#include "../SourceLocation.hpp"
#include "../UString.hpp"
#include "Stream.hpp"
#include <type_traits>

namespace Util {

class BufferedReader {
public:
    explicit BufferedReader(ReadableStream& stream)
        : m_stream(stream) {
    }

    ReadableStream& stream() const { return m_stream; }

    bool is_eof() const;
    OsErrorOr<size_t> read(std::span<uint8_t>);
    OsErrorOr<bool> read_all(std::span<uint8_t>);
    OsErrorOr<std::optional<uint8_t>> get();
    OsErrorOr<std::optional<uint8_t>> peek();
    OsErrorOr<void> seek(ssize_t offset, SeekDirection = SeekDirection::FromCurrent);

private:
    bool buffer_is_empty() const { return m_buffer_offset >= m_buffer.size(); }
    [[nodiscard]] size_t read_from_buffer(std::span<uint8_t>);
    OsErrorOr<size_t> refill_buffer();

    ReadableStream& m_stream;

    Buffer m_buffer;
    size_t m_buffer_offset = 0;
};

class BinaryReader : public BufferedReader {
public:
    explicit BinaryReader(ReadableStream& stream)
        : BufferedReader(stream) { }

    template<std::integral T>
        requires requires(T t) {
                     { convert_from_little_to_host_endian(t) } -> std::same_as<T>;
                 }
    OsErrorOr<T> read_little_endian() {
        T value;
        if (!TRY(read_all({ reinterpret_cast<uint8_t*>(&value), sizeof(T) })))
            return OsError { 0, "EOF in read_little_endian" };
        return convert_from_little_to_host_endian(value);
    }

    template<std::floating_point FP>
        requires(sizeof(FP) == 4)
    OsErrorOr<FP> read_little_endian() { return std::bit_cast<FP>(TRY(read_little_endian<uint32_t>())); }

    template<std::floating_point FP>
        requires(sizeof(FP) == 8)
    OsErrorOr<FP> read_little_endian() { return std::bit_cast<FP>(TRY(read_little_endian<uint64_t>())); }

    template<std::integral T>
        requires requires(T t) {
                     { convert_from_big_to_host_endian(t) } -> std::same_as<T>;
                 }
    OsErrorOr<T> read_big_endian() {
        T value;
        if (!TRY(read_all({ reinterpret_cast<uint8_t*>(&value), sizeof(T) })))
            return OsError { 0, "EOF in read_big_endian" };
        return convert_from_big_to_host_endian(value);
    }

    template<std::floating_point FP>
        requires(sizeof(FP) == 4)
    OsErrorOr<FP> read_big_endian() { return std::bit_cast<FP>(TRY(read_big_endian<uint32_t>())); }

    template<std::floating_point FP>
        requires(sizeof(FP) == 8)
    OsErrorOr<FP> read_big_endian() { return std::bit_cast<FP>(TRY(read_big_endian<uint64_t>())); }

    template<class T>
        requires(std::is_trivial_v<T>)
    OsErrorOr<T> read_struct() {
        T t;
        if (!TRY(read_all({ reinterpret_cast<uint8_t*>(&t), sizeof(t) }))) {
            return OsError { 0, "EOF in read_struct" };
        }
        return t;
    }

    // This reads `delim` but doesn't include it in the buffer.
    OsErrorOr<Buffer> read_until(uint8_t delim);

    template<class Callback>
    OsErrorOr<Buffer> read_while(Callback&& callback) {
        Buffer result;
        auto c = TRY(peek());
        while (c && callback(*c)) {
            result.append(*c);
            TRY(get());
            c = TRY(peek());
        }
        return result;
    }
};

class TextReader : public BufferedReader {
public:
    explicit TextReader(ReadableStream& stream, UString::Encoding encoding = UString::Encoding::Utf8)
        : BufferedReader(stream)
        , m_encoding(encoding) { }

    SourceLocation location() { return m_location; }

    bool is_eof() const {
        return BufferedReader::is_eof() && !m_peeked_codepoint;
    }

    // Peek at single codepoint without removing it from stream.
    OsErrorOr<std::optional<uint32_t>> peek();

    // Consume a single codepoint.
    OsErrorOr<std::optional<uint32_t>> consume();

    // Consumes delim but doesn't include in returned value.
    OsErrorOr<UString> consume_until(uint32_t delim);

    template<class Callback>
    OsErrorOr<UString> consume_while(Callback&& callback) {
        std::vector<uint32_t> result;
        auto c = TRY(peek());
        while (c && callback(*c)) {
            result.push_back(*c);
            TRY(consume());
            c = TRY(peek());
        }
        return UString { result };
    }

    OsErrorOr<UString> consume_line();

    // True if there was any whitespace actually read
    OsErrorOr<bool> ignore_whitespace() {
        auto result = TRY(consume_while([](uint8_t c) { return isspace(c); }));
        return result.size() > 0;
    }

private:
    OsErrorOr<std::optional<uint32_t>> consume_impl();

    std::optional<uint32_t> m_peeked_codepoint;
    SourceLocation m_location;
    UString::Encoding m_encoding {};
};

}
