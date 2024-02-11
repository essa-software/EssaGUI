#pragma once

#include <EssaUtil/Buffer.hpp>
#include <string>

namespace Util::Base64 {

std::string encode(Util::Buffer const& bin_data);
Util::Buffer decode(std::string const& encoded_string);

}
