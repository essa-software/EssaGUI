#pragma once

#include "Error.hpp"
#include "UString.hpp"

#include <sys/stat.h>

namespace Util::System {

OsErrorOr<UString> getenv_or_error(std::string const& name);
OsErrorOr<struct stat> stat(std::string const& name);

}
