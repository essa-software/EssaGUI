#include <Essa/BuildConfig.hpp>

namespace Essa::BuildConfig {

bool is_production = @ESSA_IS_PRODUCTION@;
std::string_view install_asset_root = "@ESSA_INSTALL_ASSET_ROOT@";
std::optional<std::string_view> builtin_asset_root = @ESSA_BUILTIN_ASSET_ROOT@;
std::string_view target_name = "@ESSA_TARGET_NAME@";

}
