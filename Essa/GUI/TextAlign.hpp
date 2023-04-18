#pragma once

#include <cmath>

#include <EssaUtil/Enum.hpp>
#include <optional>

namespace GUI {

#define ENUMERATE_ALIGNMENTS(A) \
    A(TopLeft)                  \
    A(Top)                      \
    A(TopRight)                 \
    A(CenterLeft)               \
    A(Center)                   \
    A(CenterRight)              \
    A(BottomLeft)               \
    A(Bottom)                   \
    A(BottomRight)

ESSA_ENUM(Align, ENUMERATE_ALIGNMENTS)
ESSA_ENUM_FROM_STRING(Align, text_align, ENUMERATE_ALIGNMENTS)

#undef ENUMERATE_ALIGNMENTS

}
