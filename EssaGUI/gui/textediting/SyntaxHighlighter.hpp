#pragma once

#include "TextPosition.hpp"
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <vector>

namespace GUI {

struct TextStyle {
    Util::Color color;
};

struct StyledTextSpan {
    size_t span_start;
    size_t span_size;

    // The resulting style of a letter is specified by highlighter.styles()[style_index].
    size_t style_index;
};

class SyntaxHighlighter {
public:
    virtual ~SyntaxHighlighter() = default;
    virtual std::vector<TextStyle> styles() const = 0;

    // Return a list of highlighted spans. Text not covered by that is
    // not highlighted, i.e. it uses theme colors.
    virtual std::vector<StyledTextSpan> spans(Util::UString const& input) const = 0;
};

}
