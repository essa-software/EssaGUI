#include "DisplayError.hpp"

#include "Stream/Reader.hpp"
#include "Stream/Stream.hpp"

namespace Util {

void display_error(ReadableStream& input, DisplayedError error) {
    fmt::print("\e[1;31mError:\e[m {}\n", error.message.encode());

    TextReader reader { input };
    size_t last_index = 0;
    while (true) {
        if (reader.is_eof()) {
            break;
        }
        auto line = MUST(reader.consume_line());

        if (last_index <= error.start_offset && last_index + line.size() >= static_cast<size_t>(error.start_offset)) {
            auto line_number = reader.is_eof() ? reader.location().line : reader.location().line - 1;
            fmt::print("{:4} | ", line_number + 1);

            size_t range_size = [&]() -> size_t {
                if (error.end_offset < error.start_offset) {
                    return line.size() - error.start_offset;
                }
                if (error.end_offset - error.start_offset < 1) {
                    return 1;
                }
                return error.end_offset - error.start_offset;
            }();

            for (ssize_t s = 0; static_cast<size_t>(s) < line.size(); s++) {
                if (s + last_index >= error.start_offset && s + last_index < error.start_offset + range_size)
                    fmt::print("\e[31m{}\e[m", Util::UString { line.at(s) }.encode());
                else
                    fmt::print("{}", Util::UString { line.at(s) }.encode());
            }
            fmt::print("\n");
            fmt::print("       ");
            for (size_t s = 0; s < error.start_offset - last_index; s++)
                fmt::print(" ");
            for (size_t s = 0; s < range_size; s++) {
                fmt::print("^");
            }
            fmt::print(" \e[31m{}\e[m\n", error.message.encode());
        }

        last_index = reader.location().offset;
    }
}

}
