#pragma once

#include "SourceLocation.hpp"
#include "Stream/Stream.hpp"
#include "UString.hpp"

namespace Util {

struct DisplayedError {
    UString message;
    size_t start_offset;
    size_t end_offset;
};

void display_error(ReadableStream& input, DisplayedError);

}
