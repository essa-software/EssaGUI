#pragma once

#include <cstddef>

namespace GUI {

struct TextPosition {
    size_t line {};
    size_t column {};

    bool operator==(TextPosition const& other) const {
        return line == other.line && column == other.column;
    }

    bool operator<(TextPosition const& other) const {
        return line < other.line || (line == other.line && column < other.column);
    }

    bool operator>(TextPosition const& other) const {
        return !(*this < other) && !(*this == other);
    }
};

struct TextSpan {
    TextPosition position;
    size_t size;
};

struct TextRange {
    TextPosition start;
    TextPosition end;
};

}
