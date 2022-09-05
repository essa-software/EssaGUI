#pragma once

#include <EssaUtil/Buffer.hpp>

namespace llgl {

namespace Clipboard {

Util::UString get_string();
void set_string(Util::UString const&);
bool has_string();

}

}
