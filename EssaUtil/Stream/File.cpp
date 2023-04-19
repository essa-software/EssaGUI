#include "File.hpp"

#include "../Config.hpp"
#include "../Error.hpp"
#include "../System.hpp"
#include "Reader.hpp"

#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

namespace Util {

File::~File() {
    if (m_owned)
        ::close(m_fd);
}

File::File(File&& other) {
    m_fd = std::exchange(other.m_fd, 0);
    m_owned = std::exchange(other.m_owned, false);
}

File& File::operator=(File&& other) {
    if (this == &other)
        return *this;
    if (m_owned)
        ::close(m_fd);
    m_fd = std::exchange(other.m_fd, 0);
    m_owned = std::exchange(other.m_owned, false);
    return *this;
}

static int seek_direction_to_c(SeekDirection dir) {
    switch (dir) {
    case SeekDirection::FromCurrent:
        return SEEK_CUR;
    case SeekDirection::FromStart:
        return SEEK_SET;
    case SeekDirection::FromEnd:
        return SEEK_END;
    }
    ESSA_UNREACHABLE;
}

OsErrorOr<void> File::seek(ssize_t count, SeekDirection direction) {
    if (lseek(m_fd, count, seek_direction_to_c(direction)) < 0) {
        return OsError { errno, "File::seek" };
    }
    return {};
}

ReadableFileStream ReadableFileStream::adopt_fd(int fd) {
    return ReadableFileStream { fd, true };
}

ReadableFileStream ReadableFileStream::borrow_fd(int fd) {
    return ReadableFileStream { fd, false };
}

OsErrorOr<ReadableFileStream> ReadableFileStream::open(std::string const& file_name) {
    auto fd = ::open(file_name.c_str(), O_RDONLY);
    if (fd < 0) {
        return OsError { errno, "WritableFileStream::open" };
    }
    return adopt_fd(fd);
}

OsErrorOr<Buffer> ReadableFileStream::read_file(std::string const& name, std::optional<size_t> max_size) {
    // TODO: Disallow read_file for streams that may be "infinite" (e.g special devices)
    auto file_size = TRY(System::stat(name)).st_size;
    if (file_size <= 0) {
        return Buffer {};
    }
    if (max_size && static_cast<size_t>(file_size) > *max_size) {
        return OsError { EFBIG, "read_file" };
    }

    Buffer result;
    auto stream = TRY(ReadableFileStream::open(name));
    auto chunk = Buffer::uninitialized(4096);
    BufferedReader reader { stream };
    while (!stream.is_eof()) {
        auto bytes_read = TRY(reader.read(chunk.span()));
        result.append(chunk.span().subspan(0, bytes_read));
    }
    return result;
}

OsErrorOr<size_t> ReadableFileStream::read(std::span<uint8_t> data) {
    // TODO: Buffering
    auto result = ::read(fd(), data.data(), data.size_bytes());
    if (result < 0) {
        return OsError { .error = static_cast<error_t>(-result), .function = "FileStream::read_all" };
    }
    if (result == 0) {
        m_eof = true;
    }
    return static_cast<size_t>(result);
}

WritableFileStream WritableFileStream::adopt_fd(int fd) {
    return WritableFileStream { fd, true };
}

WritableFileStream WritableFileStream::borrow_fd(int fd) {
    return WritableFileStream { fd, false };
}

OsErrorOr<WritableFileStream> WritableFileStream::open(std::string const& file_name, OpenOptions options) {
    auto fd = ::open(file_name.c_str(), O_WRONLY | O_CREAT | (options.truncate ? O_TRUNC : O_APPEND) | (options.fail_if_exists ? O_EXCL : 0), 0700);
    if (fd < 0) {
        return OsError { errno, "WritableFileStream::open" };
    }
    return adopt_fd(fd);
}

OsErrorOr<size_t> WritableFileStream::write(std::span<uint8_t const> data) {
    // TODO: Buffering
    auto result = ::write(fd(), data.data(), data.size_bytes());
    if (result < 0) {
        return OsError { .error = static_cast<error_t>(-result), .function = "FileStream::write_all" };
    }
    return static_cast<size_t>(result);
}

bool ReadableFileStream::is_eof() const {
    return m_eof;
}

}
