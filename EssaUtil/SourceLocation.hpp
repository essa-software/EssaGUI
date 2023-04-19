#pragma once

#include <cstddef>

namespace Util {

struct SourceLocation {
    size_t line = 0;
    size_t column = 0;
    size_t offset = 0;
};

struct SourceRange {
    SourceLocation start;
    SourceLocation end;
};

}
