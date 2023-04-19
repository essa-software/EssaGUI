#include "MemoryStream.hpp"

#include <cerrno>

namespace Util {

ReadableMemoryStream::ReadableMemoryStream(std::span<uint8_t const> data)
    : m_data { data } {
}

OsErrorOr<size_t> ReadableMemoryStream::read(std::span<uint8_t> data) {
    size_t bytes_to_read = m_offset + data.size() > m_data.size() ? m_data.size() - m_offset : data.size();
    if (bytes_to_read == 0)
        return (size_t)0;
    std::copy(m_data.begin() + m_offset, m_data.begin() + m_offset + bytes_to_read, data.data());
    m_offset += bytes_to_read;
    return bytes_to_read;
}

bool ReadableMemoryStream::is_eof() const {
    return m_offset >= m_data.size();
}

OsErrorOr<void> ReadableMemoryStream::seek(ssize_t count, SeekDirection direction) {
    auto new_offset = [&]() -> ssize_t {
        switch (direction) {
        case SeekDirection::FromCurrent:
            return (ssize_t)m_offset + count;
        case SeekDirection::FromStart:
            return count;
        case SeekDirection::FromEnd:
            return (ssize_t)m_data.size() + count;
        }
        ESSA_UNREACHABLE;
    }();
    if (new_offset < 0 || new_offset > (ssize_t)m_data.size()) {
        return OsError { EINVAL, "ReadableMemoryStream::seek" };
    }
    m_offset = new_offset;
    return {};
}

OsErrorOr<size_t> WritableMemoryStream::write(std::span<uint8_t const> data) {
    m_data.append(data);
    return data.size();
}

OsErrorOr<void> WritableMemoryStream::seek(ssize_t, SeekDirection) {
    return OsError { ENOTSUP, "WritableMemoryStream::seek: TODO" };
}

}
