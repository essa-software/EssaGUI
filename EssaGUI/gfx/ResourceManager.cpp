#include "ResourceManager.hpp"
#include "EssaGUI/util/ConfigFile.hpp"

#include <LLGL/Resources/ImageLoader.hpp>
#include <filesystem>
#include <unistd.h>

namespace Gfx {

ResourceManager::ResourceManager()
    : m_config {
        (find_resource_roots(), Util::ConfigFile::open_ini(require_lookup_resource("Config.ini", ResourceType::Generic)).release_value())
    } { }

std::optional<std::string> ResourceManager::lookup_resource(std::string const& path, ResourceType resource_type) const {
    auto base_directory = [resource_type]() {
        switch (resource_type) {
        case ResourceType::Texture:
            return "textures";
        case ResourceType::Font:
            return "fonts";
        case ResourceType::Data:
            return "data";
        case ResourceType::Generic:
            return "";
        }
        __builtin_unreachable();
    }();

    for (auto const& root : m_resource_roots) {
        auto filesystem_path = std::filesystem::path { root } / base_directory / path;
        if (std::filesystem::exists(filesystem_path))
            return filesystem_path;
    }
    return {};
}

std::string ResourceManager::require_lookup_resource(std::string const& path, ResourceType type) const {
    auto fs_path = lookup_resource(path, type);
    if (!fs_path) {
        auto resource_name = [type]() {
            switch (type) {
            case ResourceType::Texture:
                return "texture";
            case ResourceType::Font:
                return "font";
            case ResourceType::Data:
                return "data file";
            case ResourceType::Generic:
                return "resource";
            }
            __builtin_unreachable();
        }();

        std::cout << "ResourceManager: Failed to lookup required " << resource_name << " " << path << ", aborting!" << std::endl;
        exit(1);
    }
    return *fs_path;
}

llgl::opengl::Texture& ResourceManager::require_texture(std::string const& path) const {
    auto cached_texture = m_cached_textures.find(path);
    if (cached_texture != m_cached_textures.end())
        return cached_texture->second;

    auto image = llgl::ImageLoader::load_from_file(require_lookup_resource(path, ResourceType::Texture));
    if (!image) {
        std::cout << "ResourceManager: Failed to load required texture '" << path << "'. Aborting" << std::endl;
        exit(1);
    }
    cached_texture = m_cached_textures.insert({ path, llgl::opengl::Texture::create_from_image(*image) }).first;
    return cached_texture->second;
}

llgl::TTFFont& ResourceManager::require_font(std::string const& path) const {
    auto cached_font = m_cached_fonts.find(path);
    if (cached_font != m_cached_fonts.end())
        return cached_font->second;

    cached_font = m_cached_fonts.insert({ path, llgl::TTFFont::open_from_file(require_lookup_resource(path, ResourceType::Font)) }).first;
    return cached_font->second;
}

llgl::TTFFont& ResourceManager::font() const {
    return require_font(m_config.get("DefaultFont").value_or("font.ttf"));
}

llgl::TTFFont& ResourceManager::bold_font() const {
    return require_font(m_config.get("DefaultBoldFont").value_or("bold-font.ttf"));
}

llgl::TTFFont& ResourceManager::fixed_width_font() const {
    return require_font(m_config.get("DefaultFixedWidthFont").value_or("fixed-width-font.ttf"));
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
