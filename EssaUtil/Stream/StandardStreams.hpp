#pragma once

#include "File.hpp"

namespace Util {

ReadableFileStream& std_in();
WritableFileStream& std_out();
WritableFileStream& std_err();

}
