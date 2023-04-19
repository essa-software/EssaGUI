#include "Reader.hpp"

namespace Util {

constexpr auto BufferSize = 4096;

bool BufferedReader::is_eof() const {
    return m_stream.is_eof() && buffer_is_empty();
}

OsErrorOr<size_t> BufferedReader::read(std::span<uint8_t> data) {
    auto read = read_from_buffer(data);
    if (read < data.size() && !m_stream.is_eof()) {
        if (data.size() - read > BufferSize) {
            return m_stream.read(data.subspan(read));
        }
        TRY(refill_buffer());
        return read_from_buffer(data.subspan(read));
    }

    return read;
}

size_t BufferedReader::read_from_buffer(std::span<uint8_t> data) {
    if (buffer_is_empty())
        return 0;
    auto to_read = std::min(m_buffer.size() - m_buffer_offset, data.size());
    std::copy(m_buffer.begin() + m_buffer_offset, m_buffer.begin() + m_buffer_offset + to_read, data.begin());
    m_buffer_offset += to_read;
    return to_read;
}

OsErrorOr<size_t> BufferedReader::refill_buffer() {
    m_buffer.resize_uninitialized(BufferSize);
    m_buffer_offset = 0;
    auto read = TRY(m_stream.read(m_buffer.span()));
    m_buffer.resize_uninitialized(read);
    return read;
}

OsErrorOr<bool> BufferedReader::read_all(std::span<uint8_t> data) {
    size_t bytes_read = 0;
    while (bytes_read < data.size()) {
        auto bytes = TRY(read(data.subspan(bytes_read)));
        if (bytes == 0)
            return false;
        bytes_read += bytes;
    }
    return true;
}

OsErrorOr<std::optional<uint8_t>> BufferedReader::get() {
    uint8_t byte;
    auto bytes_read = TRY(read_all({ &byte, 1 }));
    if (!bytes_read)
        return std::optional<uint8_t> {};
    return byte;
}

OsErrorOr<std::optional<uint8_t>> BufferedReader::peek() {
    if (buffer_is_empty()) {
        TRY(refill_buffer());
    }
    if (!buffer_is_empty()) {
        return m_buffer[m_buffer_offset];
    }
    return std::optional<uint8_t> {};
}

OsErrorOr<void> BufferedReader::seek(ssize_t offset, SeekDirection dir) {
    if (dir == SeekDirection::FromCurrent) {
        offset -= m_buffer.size() - m_buffer_offset;
    }
    m_buffer_offset = m_buffer.size();
    m_buffer = {};
    return m_stream.seek(offset, dir);
}

OsErrorOr<Buffer> BinaryReader::read_until(uint8_t delim) {
    Buffer result;
    while (true) {
        auto ch = TRY(get());
        if (!ch || *ch == delim) {
            break;
        }
        result.append(*ch);
    }
    return result;
}

OsErrorOr<std::optional<uint32_t>> TextReader::consume() {
    auto cp = TRY(consume_impl());
    if (!cp) {
        return cp;
    }
    if (*cp == '\n') {
        m_location.line++;
        m_location.column = 0;
    }
    else {
        m_location.column++;
    }
    m_location.offset++;

    return cp;
}

OsErrorOr<std::optional<uint32_t>> TextReader::consume_impl() {
    if (m_peeked_codepoint) {
        auto codepoint = *m_peeked_codepoint;
        m_peeked_codepoint = {};
        return codepoint;
    }
    std::vector<uint8_t> buffer;
    while (true) {
        auto c = TRY(get());
        if (!c) {
            return std::optional<uint32_t> {};
        }
        buffer.push_back(*c);

        // Check if we can already decode the codepoint.
        auto decoded = Util::UString::decode(buffer);
        if (!decoded.is_error()) {
            assert(decoded.value().size() == 1);
            return decoded.release_value().at(0);
        }
    }
}

OsErrorOr<std::optional<uint32_t>> TextReader::peek() {
    if (m_peeked_codepoint) {
        return m_peeked_codepoint;
    }
    m_peeked_codepoint = TRY(consume_impl());
    return m_peeked_codepoint;
}

OsErrorOr<UString> TextReader::consume_until(uint32_t delim) {
    Buffer result;
    while (true) {
        auto ch = TRY(consume());
        if (!ch || *ch == delim) {
            break;
        }
        result.append(*ch);
    }
    return result.decode(m_encoding).map_error([&](auto) {
        return OsError { .error = 0, .function = "consume_until: Decoding failed" };
    });
}

OsErrorOr<UString> TextReader::consume_line() {
    return TRY(consume_until('\n'));
}

}
