#pragma once

#include "SourceLocation.hpp"
#include "Stream/Stream.hpp"
#include "UString.hpp"

namespace Util {

struct DisplayedError {
    std::string file_name;
    UString message;
    size_t start_offset;
    size_t end_offset;
};

void display_error(ReadableStream& input, DisplayedError);

}
