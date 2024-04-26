#pragma once

#include "Error.hpp"
#include "GenericParser.hpp"
#include "SourceLocation.hpp"

namespace Util {

struct OsToParseError {
    OsToParseError(Util::SourceRange loc)
        : m_range(loc) { }

    Util::ParseError operator()(Util::OsError&& err) const {
        return { .message = std::string { err.function }, .location = m_range };
    }

    Util::SourceRange m_range;
};

}
