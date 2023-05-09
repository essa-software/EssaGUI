#pragma once

#include <optional>
#include <string_view>

namespace Essa::BuildConfig {

extern bool is_production;
extern std::string_view install_asset_root;
extern std::optional<std::string_view> builtin_asset_root;
extern std::string_view target_name;

}
