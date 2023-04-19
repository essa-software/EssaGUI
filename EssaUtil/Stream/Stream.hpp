#pragma once

#include "../Error.hpp"
#include <cstddef>
#include <cstdint>
#include <span>

namespace Util {

enum class SeekDirection {
    FromCurrent,
    FromStart,
    FromEnd
};

class ReadableStream {
public:
    virtual ~ReadableStream() = default;

    // Try to read data to buffer. Returns number of bytes read (0 on EOF)
    virtual OsErrorOr<size_t> read(std::span<uint8_t>) = 0;

    // Return true if data read failed because of end-of-file.
    virtual bool is_eof() const = 0;

    virtual OsErrorOr<void> seek(ssize_t count, SeekDirection direction = SeekDirection::FromCurrent) = 0;
};

class WritableStream {
public:
    virtual ~WritableStream() = default;

    // Try to write data to buffer. Returns number of bytes written.
    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) = 0;

    virtual OsErrorOr<void> seek(ssize_t count, SeekDirection direction = SeekDirection::FromCurrent) = 0;
};

}
