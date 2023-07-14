#include "Config.hpp"

#include <fmt/format.h>

void Util::_crash(char const* message, CppSourceLocation const& loc) {
    fmt::print(stderr, "Aborting: {} ({}:{}:{})\n", message, loc.file_name(), loc.column(), loc.line());
    abort();
}
