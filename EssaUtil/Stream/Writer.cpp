#include "Writer.hpp"

#include <fmt/core.h>

namespace Util {

OsErrorOr<size_t> Writer::write(std::span<uint8_t const> data) {
    return m_stream.write(data);
}

OsErrorOr<void> Writer::write_all(std::span<uint8_t const> data) {
    size_t bytes_written = 0;
    while (bytes_written < data.size()) {
        bytes_written += TRY(write(data.subspan(bytes_written)));
    }
    return {};
}

OsErrorOr<void> Writer::write(UString const& string) {
    auto encoded = string.encode(m_encoding);
    return write_all({ reinterpret_cast<uint8_t const*>(encoded.data()), encoded.size() });
}

void Writer::vwriteff(fmt::string_view fmtstr, fmt::format_args args) {
    struct OutputIt {
        OutputIt(WritableStream& stream)
            : m_stream(&stream) { }

        struct OutputAdapter {
            OutputAdapter(OutputIt& it)
                : m_it(it) { }

            OutputAdapter& operator=(uint8_t t) {
                (void)m_it.m_stream->write({ &t, 1 });
                return *this;
            }

            OutputIt& m_it;
        };

        OutputAdapter operator*() { return OutputAdapter { *this }; }

        auto& operator++() {
            return *this;
        }
        auto operator++(int) {
            return *this;
        }
        bool operator==(OutputIt const& other) const = default;

        WritableStream* m_stream;
    };
    OutputIt out { m_stream };
    fmt::vformat_to(out, fmtstr, std::move(args));
}

}
