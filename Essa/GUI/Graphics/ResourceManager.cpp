#include "ResourceManager.hpp"

#include <Essa/BuildConfig.hpp>
#include <Essa/GUI/Util/ConfigFile.hpp>
#include <Essa/LLGL/Resources/ImageLoader.hpp>
#include <EssaUtil/NonCopyable.hpp>
#include <filesystem>
#include <type_traits>
#include <unistd.h>

// Add some global variables that are expected to be overridden
// by apps that link to Essa.

namespace Essa::BuildConfig {

__attribute((weak)) bool is_production = false;
__attribute((weak)) std::string_view install_asset_root = "UNKNOWN";
__attribute((weak)) std::optional<std::string_view> builtin_asset_root = {};
__attribute((weak)) std::string_view target_name = "builtin";

}

extern "C" {
__attribute((weak)) const char* ESSA_RESOURCE_DIR = nullptr;
}

namespace Gfx {

ResourceManager::ResourceManager()
    : m_config { (
        find_resource_roots(), Util::ConfigFile::open_ini(require_lookup_resource({ ResourceId::asset("Config.ini"), "" })).release_value()
    ) } { }

std::optional<std::string> ResourceManager::lookup_resource(ResourceIdAndBase const& id_and_base) const {
    if (id_and_base.id.type() == ResourceId::Type::External) {
        try {
            return std::filesystem::absolute(id_and_base.id.path());
        } catch (...) {
            return {};
        }
    }

    for (auto const& root : m_resource_roots) {
        auto filesystem_path = std::filesystem::path { root } / id_and_base.base / id_and_base.id.path();
        if (std::filesystem::exists(filesystem_path))
            return filesystem_path;
    }
    return {};
}

std::string ResourceManager::require_lookup_resource(ResourceIdAndBase const& id_and_base) const {
    auto fs_path = lookup_resource(id_and_base);
    if (!fs_path) {
        std::cout << "ResourceManager: Failed to lookup required resource '" << id_and_base << "'" << std::endl;
        exit(1);
    }
    return *fs_path;
}

std::optional<Texture> ResourceTraits<Texture>::load_from_file(std::string const& path) {
    auto image = llgl::ImageLoader::load_from_file(path);
    if (!image)
        return {};
    return llgl::Texture::create_from_image(*image);
}

std::optional<Font> ResourceTraits<Font>::load_from_file(std::string const& path) { return llgl::TTFFont::open_from_file(path); }

llgl::TTFFont& ResourceManager::font() const { return require<Font>(m_config.get("DefaultFont").value_or("font.ttf")); }

llgl::TTFFont& ResourceManager::bold_font() const { return require<Font>(m_config.get("DefaultBoldFont").value_or("bold-font.ttf")); }

llgl::TTFFont& ResourceManager::fixed_width_font() const {
    return require<Font>(m_config.get("DefaultFixedWidthFont").value_or("fixed-width-font.ttf"));
}

static std::filesystem::path exec_path() {
#ifdef __linux__
    return std::filesystem::read_symlink("/proc/" + std::to_string(getpid()) + "/exe").parent_path();
#elif __EMSCRIPTEN__
    return "/";
#else
#    error OS not supported (install linux)
#endif
}

// See docs/ResourceManager.md#searching-for-resource-directory
void ResourceManager::find_resource_roots() {
    auto add_resource_root_if_is_essa_resource_root = [this](std::filesystem::path const& path) {
        if (std::filesystem::is_directory(path) && std::filesystem::is_regular_file(path / ".essaguiresources")) {
            std::cout << "ResourceManager: Adding resource root: " << path << std::endl;
            m_resource_roots.push_back(path);
        }
    };

    if (Essa::BuildConfig::is_production) {
        // For production builds, the following paths are used:
        // -   `$EXEC_DIR/assets/<target name>`,
        //     `$EXEC_DIR/../assets/<target_name>` all the way up to the root
        //      directory
        {
            auto base_path = exec_path();
            while (true) {
                add_resource_root_if_is_essa_resource_root(base_path / "assets" / Essa::BuildConfig::target_name);
                if (base_path == "/")
                    break;
                base_path = base_path.parent_path();
            }
        }

        // -   `$CMAKE_INSTALL_PREFIX/share/$CMAKE_PROJECT_NAME/<target name>`
        //     (NOTE: Exposed as Essa::BuildConfig::install_asset_root)
        add_resource_root_if_is_essa_resource_root(Essa::BuildConfig::install_asset_root);

        // -   `$CMAKE_INSTALL_PREFIX/share/Essa/builtin` (Essa builtin
        //      resources)
        //     Let's use parent paths to avoid adding another cmake variable.
        add_resource_root_if_is_essa_resource_root(
            std::filesystem::path(Essa::BuildConfig::install_asset_root).parent_path().parent_path() / "Essa" / "builtin"
        );
    }
    else {
        // For development builds, the following paths are used:
        // -   Optionally: Path specified by `essa_resources()` CMake function
        //     (absolute path to source-local asset root, baked into executable)
        if (ESSA_RESOURCE_DIR) {
            add_resource_root_if_is_essa_resource_root(ESSA_RESOURCE_DIR);
        }

        // -   $CMAKE_SOURCE_DIR/assets (Essa builtin resources)
        if (Essa::BuildConfig::builtin_asset_root) {
            add_resource_root_if_is_essa_resource_root(*Essa::BuildConfig::builtin_asset_root);
        }
    }

    if (m_resource_roots.empty()) {
        std::cerr << "ResourceManager: Failed to find any suitable resource "
                     "root. Aborting."
                  << std::endl;
        exit(1);
    }
}

}
