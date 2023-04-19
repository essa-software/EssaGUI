#pragma once

#include "../Buffer.hpp"
#include "../Error.hpp"
#include "../NonCopyable.hpp"
#include "Stream.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace Util {

class File : public NonCopyable {
public:
    File(int fd, bool owned)
        : m_fd(fd)
        , m_owned(owned) { }

    virtual ~File();
    File(File&& other);
    File& operator=(File&& other);

    int fd() const { return m_fd; }

    OsErrorOr<void> seek(ssize_t count, SeekDirection direction = SeekDirection::FromCurrent);

private:
    int m_fd {};
    bool m_owned { false };
};

class ReadableFileStream : public ReadableStream
    , public File {
public:
    static ReadableFileStream adopt_fd(int fd);
    static ReadableFileStream borrow_fd(int fd);
    static OsErrorOr<ReadableFileStream> open(std::string const& file_name);

    // Read the entire file into a buffer. This may allocate a lot for big files, so
    // you can specify max size that can be read (otherwise an error will occur).
    static OsErrorOr<Buffer> read_file(std::string const& name, std::optional<size_t> max_size = {});

    virtual OsErrorOr<size_t> read(std::span<uint8_t>) override;
    virtual bool is_eof() const override;

    virtual OsErrorOr<void> seek(ssize_t count, SeekDirection direction = SeekDirection::FromCurrent) override {
        return File::seek(count, direction);
    }

private:
    ReadableFileStream(int fd, bool owned)
        : File(fd, owned) { }

    bool m_eof { false };
};

class WritableFileStream : public WritableStream
    , public File {
public:
    static WritableFileStream adopt_fd(int fd);
    static WritableFileStream borrow_fd(int fd);
    struct OpenOptions {
        bool truncate = false;
        bool fail_if_exists = false;
    };
    static OsErrorOr<WritableFileStream> open(std::string const& file_name, OpenOptions options);

    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) override;

    virtual OsErrorOr<void> seek(ssize_t count, SeekDirection direction = SeekDirection::FromCurrent) override {
        return File::seek(count, direction);
    }

private:
    WritableFileStream(int fd, bool owned)
        : File(fd, owned) { }
};

}
