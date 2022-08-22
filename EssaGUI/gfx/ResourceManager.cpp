#include "ResourceManager.hpp"
#include "EssaGUI/util/ConfigFile.hpp"

#include <EssaUtil/NonCopyable.hpp>
#include <LLGL/Resources/ImageLoader.hpp>
#include <filesystem>
#include <type_traits>
#include <unistd.h>

namespace Gfx {

ResourceManager::ResourceManager()
    : m_config {
        (find_resource_roots(), Util::ConfigFile::open_ini(require_lookup_resource(ResourceId::asset("Config.ini"), "")).release_value())
    } { }

std::optional<std::string> ResourceManager::lookup_resource(ResourceId path, std::string_view base) const {
    if (path.type() == ResourceId::Type::External) {
        return std::filesystem::absolute(path.path());
    }

    for (auto const& root : m_resource_roots) {
        auto filesystem_path = std::filesystem::path { root } / base / path.path();
        if (std::filesystem::exists(filesystem_path))
            return filesystem_path;
    }
    return {};
}

std::string ResourceManager::require_lookup_resource(ResourceId path, std::string_view base) const {
    auto fs_path = lookup_resource(path, base);
    if (!fs_path) {
        std::cout << "ResourceManager: Failed to lookup required resource '" << path << std::endl;
        exit(1);
    }
    return *fs_path;
}

std::optional<Texture> ResourceTraits<Texture>::load_from_file(std::string const& path) {
    auto image = llgl::ImageLoader::load_from_file(path);
    if (!image)
        return {};
    return llgl::opengl::Texture::create_from_image(*image);
}

std::optional<Font> ResourceTraits<Font>::load_from_file(std::string const& path) {
    return llgl::TTFFont::open_from_file(path);
}

llgl::TTFFont& ResourceManager::font() const {
    return require<Font>(m_config.get("DefaultFont").value_or("font.ttf"));
}

llgl::TTFFont& ResourceManager::bold_font() const {
    return require<Font>(m_config.get("DefaultBoldFont").value_or("bold-font.ttf"));
}

llgl::TTFFont& ResourceManager::fixed_width_font() const {
    return require<Font>(m_config.get("DefaultFixedWidthFont").value_or("fixed-width-font.ttf"));
}

static std::filesystem::path exec_path() {
#ifdef __linux__
    return std::filesystem::read_symlink("/proc/" + std::to_string(getpid()) + "/exe").parent_path();
#else
#    error OS not supported (install linux)
#endif
}

void ResourceManager::find_resource_roots() {
    auto add_resource_root_if_is_essagui_resource_root = [this](std::filesystem::path const& path) {
        if (std::filesystem::is_directory(path) && std::filesystem::is_regular_file(path / ".essaguiresources")) {
            std::cout << "ResourceManager: Adding resource root: " << path << std::endl;
            m_resource_roots.push_back(path);
        }
    };

    // 1. `$EXEC_DIR/assets`, `$EXEC_DIR/../assets` all the way up to the root directory
    {
        auto base_path = exec_path();
        while (true) {
            add_resource_root_if_is_essagui_resource_root(base_path / "assets");
            if (base_path == "/")
                break;
            base_path = base_path.parent_path();
        }
    }

    // 2. `$CMAKE_INSTALL_PREFIX/EssaGUI/assets`
    // FIXME: Do not hardcode install prefix
    add_resource_root_if_is_essagui_resource_root("/usr/local/share/EssaGUI/assets");

    if (m_resource_roots.empty()) {
        std::cerr << "ResourceManager: Failed to find any suitable resource root. Aborting." << std::endl;
        exit(1);
    }
}

}
