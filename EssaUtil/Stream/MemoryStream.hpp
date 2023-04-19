#pragma once

#include "../Buffer.hpp"
#include "Stream.hpp"

#include <vector>

namespace Util {

class ReadableMemoryStream : public ReadableStream {
public:
    explicit ReadableMemoryStream(std::span<uint8_t const> data);

    template<size_t S>
    static ReadableMemoryStream from_string(char const (&data)[S]) {
        return ReadableMemoryStream { { reinterpret_cast<uint8_t const*>(data), S - 1 } };
    }

    static ReadableMemoryStream from_string(std::string_view str) {
        return ReadableMemoryStream { { reinterpret_cast<uint8_t const*>(str.data()), str.size() } };
    }

    static ReadableMemoryStream from_string(std::string const& str) {
        return ReadableMemoryStream { { reinterpret_cast<uint8_t const*>(str.c_str()), str.size() } };
    }

    virtual OsErrorOr<size_t> read(std::span<uint8_t>) override;
    virtual bool is_eof() const override;
    virtual OsErrorOr<void> seek(ssize_t count, SeekDirection direction = SeekDirection::FromCurrent) override;

private:
    Buffer m_data;
    size_t m_offset = 0;
};

class WritableMemoryStream : public WritableStream {
public:
    std::span<uint8_t const> data() const { return m_data.span(); }

    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) override;
    virtual OsErrorOr<void> seek(ssize_t count, SeekDirection direction = SeekDirection::FromCurrent) override;

private:
    Buffer m_data;
};

}
